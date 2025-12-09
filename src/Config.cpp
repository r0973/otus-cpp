#include <iostream>
#include <algorithm>
#include "Config.h"


bool Configuration::parse_arguments(int argc, char* argv[])
{
    po::options_description desc("Allowed options");

    // Временные переменные строкового типа для парсинга
    std::vector<std::string> temp_include_strings;
    std::vector<std::string> temp_exclude_strings;

    desc.add_options()
        ("help,h", "produce help message")
        ("include,i", po::value<std::vector<std::string>>(&temp_include_strings)->multitoken(), "directories to scan")
        ("exclude,e", po::value<std::vector<std::string>>(&temp_exclude_strings)->multitoken(), "directories to exclude")
        ("level,l", po::value<int>(&recursion_level), "scan level (0 = current dir only)")
        ("min-size,m", po::value<size_t>(&min_file_size), "minimum file size in bytes (default 1)")
        ("masks,k", po::value<std::vector<std::string>>(&file_masks)->multitoken(), "filename masks (case insensitive)")
        ("block-size,b", po::value<size_t>(&block_size), "block size S for hashing (default 4096)")
        ("hash,a", po::value<std::string>()->default_value("crc32"), "hashing algorithm (crc32, md5)");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const po::error& e)
    {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        std::cout << desc << std::endl;
        return false;
    }

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return false;
    }
    
    if (vm.count("include") == 0)
    {
        std::cerr << "Error: You must specify at least one directory to scan using -i/--include." << std::endl;
        std::cout << desc << std::endl;
        return false;
    }

    // Convert string paths to boost::filesystem::path and normalize them
    auto normalize_paths = [](const std::vector<std::string>& paths, std::vector<fs::path>& out_paths)
    {
        for (const auto& p_str : paths)
        {
            fs::path p(p_str);
            try
            {
                if (fs::exists(p) && fs::is_directory(p))
                {
                    out_paths.push_back(fs::canonical(p));
                }
                else
                {
                    std::cerr << "Warning: Directory not found or invalid: " 
                              << p_str << std::endl;
                }
            }
            catch (const fs::filesystem_error& e)
            {
                 std::cerr << "Filesystem error with path " << p_str << ": " 
                           << e.what() << std::endl;
            }
        }
    };
    
    normalize_paths(temp_include_strings, include_dirs);
    if (vm.count("exclude"))
    {
        normalize_paths(temp_exclude_strings, exclude_dirs);
    }

    std::string hash_algo_str = vm["hash"].as<std::string>();
    if (hash_algo_str == "crc32")
    {
        hash_algo = HashAlgorithm::CRC32;
    }
    else if (hash_algo_str == "md5")
    {
        hash_algo = HashAlgorithm::MD5;
        // Дополнительная проверка доступности Boost MD5
        #ifndef BOOST_UUID_MD5_HPP_INCLUDED
        std::cerr << "Warning: Boost MD5 support might not be fully available." << std::endl;
        #endif
    }
    else
    {
        std::cerr << "Invalid hash algorithm specified. Use crc32 or md5." << std::endl;
        return false;
    }

    return true;
}
