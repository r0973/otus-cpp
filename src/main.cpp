#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include "Config.h"
#include "FileScanner.h"
#include "Comparison.h"

int main(int argc, char* argv[])
{
    Configuration config;
    if (!config.parse_arguments(argc, argv))
    {
        return 1;
    }

    std::cout << "Starting file scan..." << std::endl;
    FileScanner scanner(config);
    FileGroupsBySize candidates = scanner.scan();
    std::cout << "Found " << candidates.size() 
              << " groups of files with matching sizes (total potential duplicates: "
              << std::accumulate(
                    candidates.begin(), candidates.end(), 0, 
                    [](int sum, const auto& pair){ return sum + static_cast<int>(pair.second.size()); })
              << " files)." << std::endl;

    if (candidates.empty())
    {
        std::cout << "No potential duplicates found based on initial filters." << std::endl;
        return 0;
    }

    std::cout << "Starting byte-by-byte comparison (lazy I/O)..." << std::endl;
    DuplicateFinder finder(config);
    DuplicateGroups duplicates = finder.find_duplicates(candidates);

    // Output results
    for (const auto& group : duplicates)
    {
        for (const auto& path : group)
        {
            std::cout << fs::canonical(path).string() << std::endl;
        }
        std::cout << std::endl;
    }

    if (duplicates.empty())
    {
         std::cout << "No actual duplicates found after content verification." << std::endl;
    }

    return 0;
}
