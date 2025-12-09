#pragma once

#include <vector>
#include <map>
#include "HashManager.h"
#include "FileScanner.h"


using DuplicateGroups = std::vector<std::vector<fs::path>>;

class DuplicateFinder
{
private:
    const Configuration& config_;
public:
    DuplicateFinder(const Configuration& config)
     : config_(config)
    {}
public:    
    DuplicateGroups find_duplicates(const FileGroupsBySize& candidates);

};
