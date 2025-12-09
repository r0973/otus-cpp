#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include "Config.h"


using byte = unsigned char;
using HashValue = std::vector<byte>; 

class BlockHashProvider
{
private:
    const fs::path file_path_;
    const size_t block_size_;
    const HashAlgorithm algo_;
    const size_t file_size_;
    const size_t total_blocks_;
    std::vector<HashValue> cached_hashes_;
    std::ifstream file_stream_;
private:
    HashValue compute_crc32_block(size_t block_index);
    HashValue compute_md5_block(size_t block_index);
public:
    BlockHashProvider(const fs::path& file_path, size_t block_size, HashAlgorithm algo);
    ~BlockHashProvider();
public:
    HashValue get_hash_of_block(size_t block_index);
    size_t total_blocks() const { return total_blocks_; }
    const fs::path& get_path() const { return file_path_; }
};
