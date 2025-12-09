#include <iostream>
#include <algorithm>
#include <map>
#include "Comparison.h"


DuplicateGroups DuplicateFinder::find_duplicates(const FileGroupsBySize& candidates)
{
    DuplicateGroups final_duplicates;

    for (const auto& [size, paths_list] : candidates)
    {
        if (paths_list.size() < 2)
            continue;

        std::vector<std::unique_ptr<BlockHashProvider>> providers;
        std::map<fs::path, size_t> path_to_index;
        
        for (size_t i = 0; i < paths_list.size(); ++i)
        {
            try
            {
                providers.push_back(std::make_unique<BlockHashProvider>(
                    paths_list[i], config_.block_size, config_.hash_algo));
                path_to_index[paths_list[i]] = i;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Cannot create hash provider for " 
                          << paths_list[i] << ": " << e.what() << std::endl;
            }
        }

        if (providers.empty())
            continue;

        std::vector<std::vector<fs::path>> current_groups = {paths_list};
        
        size_t total_blocks = providers[0]->total_blocks();
        for (size_t block_idx = 0; block_idx < total_blocks; ++block_idx)
        {
            std::vector<std::vector<fs::path>> next_groups;
            for (const auto& group : current_groups)
            {
                if (group.size() < 2)
                    continue;
                
                std::map<HashValue, std::vector<fs::path>> split_map;
                
                for (const auto& path : group)
                {
                    auto it = path_to_index.find(path);
                    if (it != path_to_index.end())
                    {
                        try
                        {
                            HashValue hash = providers[it->second]->get_hash_of_block(block_idx);
                            split_map[hash].push_back(path);
                        }
                        catch (const std::exception& e)
                        {
                            std::cerr << "Error hashing block " << block_idx 
                                      << " for " << path << ": " << e.what() << std::endl;
                        }
                    }
                }
                
                // Добавляем только группы с >1 файлом
                for (auto& [hash, sub_group] : split_map)
                {
                    if (sub_group.size() > 1)
                    {
                        next_groups.push_back(std::move(sub_group));
                    }
                }
            }
            
            if (next_groups.empty())
                break;
            
            current_groups = std::move(next_groups);
        }
        
        for (auto& group : current_groups)
        {
            if (group.size() > 1)
            {
                final_duplicates.push_back(std::move(group));
            }
        }
    }

    return final_duplicates;
}