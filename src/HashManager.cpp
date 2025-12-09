
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <boost/endian/conversion.hpp>
#include "HashManager.h"

BlockHashProvider::BlockHashProvider(const fs::path& file_path, size_t block_size, HashAlgorithm algo)
: file_path_{file_path}
, block_size_{block_size}
, algo_{algo}
, file_size_{fs::file_size(file_path_)}
, total_blocks_{(file_size_ + block_size_ - 1) / block_size_}
{
    cached_hashes_.resize(total_blocks_); 
    file_stream_.open(file_path_, std::ifstream::binary);
    if (!file_stream_.is_open())
    {
        std::cerr << "Error: Could not open file for reading: " << file_path_ << std::endl;
    }
}

BlockHashProvider::~BlockHashProvider()
{
    if (file_stream_.is_open())
    {
        file_stream_.close();
    }
}

HashValue BlockHashProvider::get_hash_of_block(size_t block_index)
{
    if (block_index >= total_blocks_)
    {
        throw std::out_of_range("Block index out of range");
    }
    
    if (!cached_hashes_[block_index].empty())
    {
        return cached_hashes_[block_index];
    }

    if (!file_stream_.is_open())
    {
        throw std::runtime_error("File stream is not open for reading.");
    }

    HashValue hash;
    switch (algo_)
    {
        case HashAlgorithm::CRC32:
            hash = compute_crc32_block(block_index); // returns 4 bytes
            break;
        case HashAlgorithm::MD5:
            hash = compute_md5_block(block_index); // returns 16 bytes
            break;
    }

    // Store in cache
    cached_hashes_[block_index] = hash;
    return hash;
}

HashValue BlockHashProvider::compute_crc32_block(size_t block_index)
{
    std::vector<char> buffer(block_size_, 0);
    size_t bytes_to_read = block_size_;
    size_t current_pos = block_index * block_size_;

    if (current_pos >= file_size_)
    {
        return HashValue{}; 
    }
    
    if (current_pos + block_size_ > file_size_)
    {
        bytes_to_read = file_size_ - current_pos;
    }

    file_stream_.clear(); 
    file_stream_.seekg(static_cast<std::streamoff>(current_pos), std::ios_base::beg);
    
    if (!file_stream_)
    {
        throw std::runtime_error("Failed to seek file stream to position " + 
                                 std::to_string(current_pos) + 
                                 " for file: " + file_path_.string());
    }

    file_stream_.read(buffer.data(), bytes_to_read);
    
    if (!file_stream_ && !file_stream_.eof())
    {
        throw std::runtime_error("Failed to read " + 
                                 std::to_string(bytes_to_read) + 
                                 " bytes from file " + file_path_.string() + 
                                 " at position " + std::to_string(current_pos));
    }
        
    boost::crc_32_type result_crc;
    result_crc.process_bytes(buffer.data(), block_size_);
    uint32_t checksum_val = result_crc.checksum();
    
    HashValue hash(sizeof(uint32_t));
    std::memcpy(hash.data(), &checksum_val, sizeof(uint32_t));
    
    return hash;
}

HashValue BlockHashProvider::compute_md5_block(size_t block_index)
{
    std::vector<char> buffer(block_size_, 0);
    size_t bytes_to_read = block_size_;
    size_t current_pos = block_index * block_size_;

    if (current_pos >= file_size_)
    { 
        return HashValue();
    }
    if (current_pos + block_size_ > file_size_)
    { 
        bytes_to_read = file_size_ - current_pos;
    }

    file_stream_.clear(); 
    file_stream_.seekg(static_cast<std::streamoff>(current_pos), std::ios_base::beg);
    
    if (!file_stream_)
    { 
        throw std::runtime_error("Failed to seek file stream.");
    }

    file_stream_.read(buffer.data(), bytes_to_read);
    if (!file_stream_ && !file_stream_.eof())
    {
        throw std::runtime_error("Failed to read bytes.");
    }

    // --> Using Boost MD5 <--
    boost::uuids::detail::md5 hash_state;
    boost::uuids::detail::md5::digest_type digest;
    
    hash_state.process_bytes(buffer.data(), block_size_);
    
    hash_state.get_digest(digest);

    HashValue hash(16);
    for(int i = 0; i < 4; ++i)
    {
        uint32_t native_word = boost::endian::little_to_native(digest[i]);
        std::memcpy(hash.data() + i * sizeof(uint32_t), &native_word, sizeof(uint32_t));
    }
    
    return hash;
}