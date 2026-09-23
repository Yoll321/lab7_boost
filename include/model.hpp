#pragma once

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/system.hpp>
#include <vector>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace s  = boost::system;

class IModel {
public:
    virtual ~IModel() = default;
    virtual void getVM(int argc, char* argv[]) = 0;
    virtual void getFilenames(void) = 0;
};

class Model : public IModel {
public:
    Model() = default;
    void getVM(int argc, char* argv[]) override;
    void getFilenames(void) override;
private:
    bool excluded(const fs::path& path) const;
    bool matchesMasks(const fs::path& path) const;

    po::variables_map vm_;
    std::vector<fs::path> filepaths_;
};