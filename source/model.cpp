#include <iostream>
#include <climits>

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
        ("block-size,S", po::value<std::size_t>()->required(), "blocl size for hashing")
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

bool Model::excluded(const fs::path& dirpath) const
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
        // std::cout << filename << ' ' << mask << ": ";
        if (boost::regex_match(filename, mask)) {
            // std::cout << "true\n";
            return true;
        }

        // std::cout << "false\n";
    }

    return false;
}

void Model::getFilenames(void)
{
    int max_depth = vm_["depth"].as<int>();
    if (max_depth < 0) max_depth = INT_MAX;
    
    std::vector<fs::path> paths;
    if (vm_.count("path") == 0)
        paths.push_back(fs::path("."));
    else for (const auto& entry : vm_["path"].as<std::vector<std::string>>()) {
        if (fs::is_directory(entry) && !excluded(entry))
            paths.push_back(fs::path(entry));
    }

    s::error_code ec;
    for (const auto& path : paths) {
        for (fs::recursive_directory_iterator it(path, ec);
            it != fs::recursive_directory_iterator();
            it.increment(ec)) 
        {
            if (ec) {
                std::cerr << ec.what() << '\n';
                ec.clear();
                continue;
            }

            if (fs::is_regular_file(*it) && matchesMasks(*it))
                filepaths_.push_back(*it);
            if (fs::is_directory(*it) && (it.depth() >= max_depth || excluded(*it)))
                it.disable_recursion_pending();
        }
    }

    for (auto& it : filepaths_)
        std::cout << it << '\n';
    
    return;
}
