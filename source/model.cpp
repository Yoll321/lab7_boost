#include <iostream>
#include <climits>
#include <fstream>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/functional/hash.hpp>

#include "model.hpp"

void Model::getVM(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("path,P", po::value<std::vector<std::string>>()->multitoken(), "paths for scanning")
        ("exclude,E", po::value<std::vector<std::string>>()->multitoken(), "path to exclude from scanning")
        ("depth,D", po::value<int>()->default_value(-1), "scanning depth (0 for no recursion, unset for unlimited)")
        ("min-size", po::value<std::size_t>()->default_value(1), "minimum file size to scan")
        ("masks", po::value<std::vector<std::string>>()->multitoken(), "masks of filenames to scan")
        ("block-size,S", po::value<std::size_t>()->required(), "block size for hashing")
        ("hash,H", po::value<std::string>()->default_value("crc32"), "hashing algorithm (crc32 or md5)")
    ;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm_);
        po::notify(vm_);
    } catch (const po::error &err){
        std::cerr << err.what() << '\n';
        std::cout << desc << '\n';
        exit(1);
    }
}

void Model::getHashFunc(void)
{
    std::string funcName = vm_["hash"].as<std::string>();
    std::transform(funcName.begin(), funcName.end(), funcName.begin(), 
        [](unsigned char c){ return std::tolower(c); });
    
    if (funcName == "crc32") hashFunc_ = HashFunc::CRC32;
    else if (funcName == "md5") hashFunc_ = HashFunc::MD5;
    else {
        std::cerr << "Unknown hash algorithm, using CRC32 instead\n";
        hashFunc_ = HashFunc::CRC32;
    }
}

bool Model::excluded(const fs::path &dirpath) const
{
    if (vm_.count("exclude") == 0) return false;
    static std::vector<fs::path> excluded;
    if (excluded.empty())
        for (const auto& p : vm_["exclude"].as<std::vector<std::string>>())
            excluded.push_back(fs::path(p));

    for (const auto& excluded_path : excluded) {
        if (fs::exists(dirpath) && fs::exists(excluded_path) && fs::equivalent(dirpath, excluded_path))
            return true;
    }

    return false;
}

std::string wildcard_to_regex(const std::string& mask) {
    std::string regex_mask;
    for (char c : mask) {
        switch (c) {
            case '*': regex_mask += ".*"; break;
            case '?': regex_mask += '.';  break;
            case '.': regex_mask += "\\."; break;
            default:  regex_mask += c;    break;
        }
    }
    return "^" + regex_mask + "$";
}

bool Model::matchesMasks(const fs::path& filepath) const {
    if (!fs::is_regular_file(filepath)) return false;
    if (vm_.count("masks") == 0) return true;

    static std::vector<boost::regex> masks;
    if (masks.empty()) {
        for (const auto& wildcard : vm_["masks"].as<std::vector<std::string>>()) {
            boost::regex new_regex(wildcard_to_regex(wildcard), boost::regex::icase);
            masks.push_back(new_regex);
        }
    }

    std::string filename = filepath.filename().string();
    for (const auto& mask : masks) {
        if (boost::regex_match(filename, mask)) {
            return true;
        }
    }

    return false;
}

void Model::getFilenames(void)
{
    int max_depth = vm_["depth"].as<int>();
    std::size_t min_size = vm_["min-size"].as<std::size_t>();
    if (max_depth < 0) max_depth = INT_MAX;
    
    std::vector<fs::path> paths;
    if (vm_.count("path") == 0)
        paths.push_back(fs::path("."));
    else for (const auto& entry : vm_["path"].as<std::vector<std::string>>()) {
        if (fs::is_directory(entry) && !excluded(entry))
            paths.push_back(fs::path(entry));
    }
    if (paths.empty()) {
        std::cerr << "Incorrect paths provided\n";
        exit(2);
    }

    s::error_code ec;
    for (const auto& path : paths) {
        for (fs::recursive_directory_iterator it(path, ec);
            it != fs::recursive_directory_iterator();
            it.increment(ec)) 
        {
            if (ec) {
                std::cerr << ec.message() << '\n';
                ec.clear();
                continue;
            }

            if (fs::is_regular_file(*it) && matchesMasks(*it) && fs::file_size(*it) >= min_size)
                filepaths_.push_back(*it);
            if (fs::is_directory(*it) && (it.depth() >= max_depth || excluded(*it)))
                it.disable_recursion_pending();
        }
    }
    
    return;
}

std::vector<unsigned char> Model::getBlockHash(const std::vector<char>& block, std::size_t bytes_read) {
    if (hashFunc_ == HashFunc::CRC32) {
        boost::crc_32_type crc;
        crc.process_bytes(block.data(), bytes_read);
        std::uint32_t checksum = crc.checksum();
        std::vector<unsigned char> out(sizeof(checksum));
        std::memcpy(out.data(), &checksum, sizeof(checksum));
        return out;
    }

    else if (hashFunc_ == HashFunc::MD5) {
        boost::uuids::detail::md5 hash;
        boost::uuids::detail::md5::digest_type digest;

        hash.process_bytes(block.data(), bytes_read);
        hash.get_digest(digest);

        std::vector<unsigned char> out(sizeof(digest));
        memcpy(out.data(), digest, sizeof(digest));
        return out;
    }

    return std::vector<unsigned char>();
}

void Model::findDuplicates(void)
{
    block_size_ = vm_["block-size"].as<std::size_t>();
    std::unordered_map<std::size_t, std::vector<fs::path>> bySize;

    struct ScanEntry {
        std::ifstream fileStream;
        fs::path      filePath;
        ScanEntry(std::ifstream fileStream, fs::path filePath) :
            fileStream(std::move(fileStream)), filePath(std::move(filePath)) {}
    };

    std::deque<std::vector<ScanEntry>> entryClasters;

    // Division of files by size
    for (const auto& filepath : filepaths_)
        bySize[fs::file_size(filepath)].push_back(filepath);
    for (const auto& [size, filepaths] : bySize) {
        if (filepaths.size() <= 1) continue;
        std::vector<ScanEntry> newClaster;
        newClaster.reserve(filepaths.size());
        for (const auto& filepath : filepaths) {
            newClaster.emplace_back(std::ifstream(filepath.string(), std::ios::binary), filepath);
        }
        entryClasters.push_back(std::move(newClaster));
    }

    while (!entryClasters.empty()) {
        auto claster = std::move(entryClasters.front());
        entryClasters.pop_front();
        std::unordered_map<std::vector<unsigned char>, std::vector<ScanEntry>, 
            boost::hash<std::vector<unsigned char>>> subClasters;
        bool clasterScanFinished = false;

        for (auto& entry : claster) {
            std::vector<char> block;
            block.resize(block_size_);
            entry.fileStream.read(block.data(), block_size_);
            std::streamsize bytesRead = entry.fileStream.gcount();

            // Reached the end of the files (since every claster constains only same-sized files)
            if (bytesRead == 0) {
                std::vector<fs::path> newDuplicateClaster;
                for (auto& [stream, path] : claster)
                    newDuplicateClaster.push_back(path);
                duplicatesTable_.push_back(std::move(newDuplicateClaster));
                clasterScanFinished = true;
                break;
            }

            subClasters[getBlockHash(block, bytesRead)].push_back(std::move(entry));
        }

        if (clasterScanFinished) continue;

        for (auto& [hash, entryClaster] : subClasters) {
            if (entryClaster.size() <= 1) continue;
            entryClasters.push_back(std::move(entryClaster));
        }
    }
    return;
}

std::vector<std::vector<fs::path>> Model::getDuplicatesTable(void) const
{
    return duplicatesTable_;
}
