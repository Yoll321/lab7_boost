#pragma once

#include "boost_includes.hpp"
#include <vector>
#include <deque>

class IModel {
public:
    virtual ~IModel() = default;
    virtual void getVM(int argc, char* argv[]) = 0;
    virtual void getFilenames(void) = 0;
    virtual void findDuplicates(void) = 0;
    virtual std::vector<std::vector<fs::path>> getDuplicatesTable(void) const = 0;
};

class Model : public IModel {
public:
    Model() = default;
    void getVM(int argc, char* argv[]) override;
    void getFilenames(void) override;
    void findDuplicates(void) override;
    std::vector<std::vector<fs::path>> getDuplicatesTable(void) const override;

private:
    enum HashFunc {
        CRC32,
        MD5
    };
    void getHashFunc(void);
    bool excluded(const fs::path& path) const;
    bool matchesMasks(const fs::path& path) const;
    std::vector<unsigned char> getBlockHash(const std::vector<char> &data, std::size_t bytes_read);

    po::variables_map vm_;
    std::size_t block_size_;
    HashFunc hashFunc_;
    std::vector<fs::path> filepaths_;
    std::vector<std::vector<fs::path>> duplicatesTable_;
};