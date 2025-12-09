#pragma once

#include <map>
#include <vector>
#include <string>
#include <regex>
#include <boost/filesystem.hpp>
#include "Config.h"


namespace fs = boost::filesystem;

using FileGroupsBySize = std::map<size_t, std::vector<fs::path>>;

class FileScanner
{
private:
    const Configuration& config_;

public:
    FileScanner(const Configuration& config)
      : config_(config)
    {}

public:    
    FileGroupsBySize scan();

private:
    void process_file_entry(const fs::directory_entry& entry, FileGroupsBySize& groups);
    FileGroupsBySize filter_single_file_groups(const FileGroupsBySize& groups);
    bool is_excluded(const fs::path& p) const;
    bool matches_mask(const std::string& filename) const;
    bool is_valid_file(const fs::path& p) const;
};
