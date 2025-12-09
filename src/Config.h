#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

enum class HashAlgorithm { CRC32, MD5 };

struct Configuration
{
    std::vector<fs::path> include_dirs;
    std::vector<fs::path> exclude_dirs;
    int recursion_level = -1; // -1 means infinite recursion
    size_t min_file_size = 1; // default > 1 byte
    std::vector<std::string> file_masks;
    size_t block_size = 4096; // S
    HashAlgorithm hash_algo = HashAlgorithm::CRC32; // Default H

    // Helper to parse CLI arguments
    bool parse_arguments(int argc, char* argv[]);
};
