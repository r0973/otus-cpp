#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include "FileScanner.h"


// Вспомогательные функции
void FileScanner::process_file_entry(const fs::directory_entry& entry, FileGroupsBySize& groups)
{
    if (fs::is_regular_file(entry.status()) && 
       !fs::is_symlink(entry.status()) && // Пропустить симлинки
        is_valid_file(entry.path()))
    {
        try
        {
            size_t file_size = fs::file_size(entry.path());
            groups[file_size].push_back(fs::canonical(entry.path()));
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Cannot read file size for " << entry.path() 
                      << ": " << e.what() << std::endl;
        }
    }
}

FileGroupsBySize FileScanner::filter_single_file_groups(const FileGroupsBySize& groups)
{
    FileGroupsBySize filtered;
    for (const auto& [size, paths] : groups)
    {
        if (paths.size() > 1)
        {
            filtered[size] = paths;
        }
    }
    return filtered;
}

FileGroupsBySize FileScanner::scan()
{
    FileGroupsBySize groups;

    for (const auto& root_dir : config_.include_dirs)
    {
        if (!fs::exists(root_dir) || !fs::is_directory(root_dir))
            continue;

        try
        {
            if (config_.recursion_level == 0)
            {
                for (const auto& entry : fs::directory_iterator(root_dir))
                {
                    process_file_entry(entry, groups);
                }
            }
            else
            {
                // Рекурсивный обход с ограничением глубины
                fs::recursive_directory_iterator it(root_dir), end;
                while(it != end)
                {
                    if (fs::is_directory(it->status()) && is_excluded(it->path()))
                    {
                        it.disable_recursion_pending(); // Пропустить исключенную директорию
                    }
                    
                    if (fs::is_regular_file(it->status()))
                    {
                        process_file_entry(*it, groups);
                    }
                    
                    // Ограничение глубины рекурсии
                    if (config_.recursion_level > 0 && 
                        it.depth() >= config_.recursion_level - 1)
                    {
                        it.pop();
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error scanning directory " << root_dir << ": " 
                      << e.what() << std::endl;
        }
    }

    // Фильтрация групп (только с >1 файлом)
    return filter_single_file_groups(groups);
}

bool FileScanner::is_excluded(const fs::path& p) const
{
    try
    {
        fs::path canonical_p = fs::canonical(p);
        for (const auto& exclude_path : config_.exclude_dirs)
        {
            try
            {
                fs::path canonical_exclude = fs::canonical(exclude_path);
                // Проверяем, что p находится внутри exclude_path
                auto res = std::search(
                    canonical_p.begin(), canonical_p.end(),
                    canonical_exclude.begin(), canonical_exclude.end()
                );
                if (res == canonical_p.begin())
                {
                    return true; // p внутри exclude_path
                }
            }
            catch (const fs::filesystem_error&)
            {
                // Пропустить недоступный exclude_path
                continue;
            }
        }
    }
    catch (const fs::filesystem_error&)
    {
        ;// пропускаем проверку
    }
    return false;
}

bool FileScanner::matches_mask(const std::string& filename) const
{
    if (config_.file_masks.empty())
        return true;

    // Convert filename to lower case for case-insensitive match
    std::string lower_filename = filename;
    std::transform(lower_filename.begin(), lower_filename.end()
                                         , lower_filename.begin(), ::tolower);

    for (const auto& mask : config_.file_masks)
    {
        std::string lower_mask = mask;
        std::transform(lower_mask.begin(), lower_mask.end(), lower_mask.begin(), ::tolower);
        
        std::string regex_pattern;
        for(char c : lower_mask)
        {
            if (c == '*')
                regex_pattern += ".*";
            else if (c == '?')
                regex_pattern += ".";
            else if (c == '.' || c == '\\' || c == '+' || 
                     c == '[' || c == ']'  || c == '(' ||
                     c == ')' || c == '{' || c == '}'  || 
                     c == '$' || c == '^')
            {
                regex_pattern += "\\";
                regex_pattern += c;
            }
            else
            {
                regex_pattern += c;
            }
        }
        
        regex_pattern = "^" + regex_pattern + "$";

        try
        {
            boost::regex pattern(regex_pattern, boost::regex::icase); 
            if (boost::regex_match(lower_filename, pattern))
            {
                return true;
            }
        }
        catch (const boost::regex_error& e)
        {
            std::cerr << "Boost.Regex error with pattern " << regex_pattern << ": " << e.what() << std::endl;
        }
    }
    return false;
}

bool FileScanner::is_valid_file(const fs::path& p) const
{
    if (is_excluded(p))
        return false;
    
    if (fs::file_size(p) < config_.min_file_size)
        return false;
    
    if (!matches_mask(p.filename().string()))
        return false;

    return true;
}
