#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/uuid/detail/md5.hpp>
#include "HashManager.h" 

BOOST_TEST_DONT_PRINT_LOG_VALUE(HashValue);

namespace fs = boost::filesystem;

void create_test_file(const fs::path& path, const std::string& content)
{
    std::ofstream ofs(path.string(), std::ios::binary);
    ofs << content;
    ofs.close();
}

HashValue create_crc_hash_value(uint32_t value)
{
    uint32_t little_endian_value = boost::endian::native_to_little(value);
    HashValue hash(sizeof(uint32_t));
    std::memcpy(hash.data(), &little_endian_value, sizeof(uint32_t));
    return hash;
}

HashValue calculate_boost_md5(const std::vector<char>& data)
{
    boost::uuids::detail::md5 hash_state;
    boost::uuids::detail::md5::digest_type digest;
    
    hash_state.process_bytes(data.data(), data.size());
    hash_state.get_digest(digest);
    
    HashValue hash(16);
    for(int i = 0; i < 4; ++i)
    {
        uint32_t native_word = boost::endian::little_to_native(digest[i]);
        std::memcpy(hash.data() + i * sizeof(uint32_t), &native_word, sizeof(uint32_t));
    }
    return hash;
}

HashValue create_md5_hash_value(const std::string& hex_string)
{
    HashValue hash(16);
    for (size_t i = 0; i < 16; ++i)
    {
        std::string byte_str = hex_string.substr(i * 2, 2);
        hash[i] = static_cast<unsigned char>(std::stoul(byte_str, nullptr, 16));
    }
    return hash;
}

std::ostream& operator<<(std::ostream& os, const byte& b)
{
    os << static_cast<int>(b); 
    return os;
}

std::ostream& operator<<(std::ostream& os, const HashValue& v)
{
    os << "{ ";
    for (size_t i = 0; i < v.size(); ++i)
    {
        os << static_cast<int>(v[i]); 
        if (i < v.size() - 1)
        {
            os << ", ";
        }
    }
    os << " }";
    return os;
}

BOOST_AUTO_TEST_SUITE(HashManagerTests)

BOOST_AUTO_TEST_CASE(PaddingAndCrcTest)
{
    fs::path test_dir = fs::temp_directory_path() / "bayan_test_temp";
    fs::create_directories(test_dir);
    fs::path file_path = test_dir / "world.txt";

    create_test_file(file_path, "Hello, Worl\n"); 

    size_t block_size = 5; // S = 5
    
    BlockHashProvider provider(file_path, block_size, HashAlgorithm::CRC32);

    BOOST_CHECK_EQUAL(provider.total_blocks(), 3); 

    // Проверяем первый блок: "Hello" 
    auto expected0 = create_crc_hash_value(4157704578U); 
    auto actual0 = provider.get_hash_of_block(0);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual0.begin(), actual0.end(), expected0.begin(), expected0.end());

    // Проверяем второй блок: ", Wor" 
    auto expected1 = create_crc_hash_value(1471989099U);
    auto actual1 = provider.get_hash_of_block(1);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual1.begin(), actual1.end(), expected1.begin(), expected1.end());

    // Проверяем третий (padded) блок: "l\n\0\0\0" 
    auto expected2 = create_crc_hash_value(4116548917U);
    auto actual2 = provider.get_hash_of_block(2);
    BOOST_CHECK_EQUAL_COLLECTIONS(actual2.begin(), actual2.end(), expected2.begin(), expected2.end());

    fs::remove_all(test_dir);
}

BOOST_AUTO_TEST_CASE(IdenticalFilesTest)
{
    fs::path test_dir = fs::temp_directory_path() / "bayan_test_temp_identical";
    fs::create_directories(test_dir);
    fs::path file1_path = test_dir / "file1.bin";
    fs::path file2_path = test_dir / "file2.bin";

    std::string content = "Some identical binary data here.";
    create_test_file(file1_path, content);
    create_test_file(file2_path, content);

    size_t block_size = 16; 
    BlockHashProvider p1(file1_path, block_size, HashAlgorithm::CRC32);
    BlockHashProvider p2(file2_path, block_size, HashAlgorithm::CRC32);

    BOOST_CHECK_EQUAL(p1.total_blocks(), p2.total_blocks());
    
    for(size_t i = 0; i < p1.total_blocks(); ++i)
    {
        BOOST_CHECK_EQUAL(p1.get_hash_of_block(i), p2.get_hash_of_block(i));
    }

    fs::remove_all(test_dir);
}

BOOST_AUTO_TEST_CASE(MD5Test)
{
    fs::path test_dir = fs::temp_directory_path() / "bayan_test_temp_md5";
    fs::create_directories(test_dir);
    fs::path file_path = test_dir / "test_md5.txt";

    std::string content = "Hello, world!";
    create_test_file(file_path, content); 

    size_t block_size = 64; // MD5 работает с блоками 64 байта
    
    BlockHashProvider provider(file_path, block_size, HashAlgorithm::MD5);

    std::vector<char> block(block_size, 0);
    std::copy(content.begin(), content.end(), block.begin());
    
    HashValue expected_md5_block = calculate_boost_md5(block);
    HashValue actual_md5_block = provider.get_hash_of_block(0);

    BOOST_CHECK_EQUAL(actual_md5_block.size(), 16); 
    
    std::stringstream expected_hex, actual_hex;
    for(int i = 0; i < 16; i++)
    {
        expected_hex << std::hex << std::setw(2) << std::setfill('0') 
                    << static_cast<int>(expected_md5_block[i]);
        actual_hex << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(actual_md5_block[i]);
    }
    
    BOOST_TEST_MESSAGE("Expected Boost MD5: " << expected_hex.str());
    BOOST_TEST_MESSAGE("Actual MD5: " << actual_hex.str());
    
    BOOST_CHECK_EQUAL_COLLECTIONS(
        actual_md5_block.begin(), actual_md5_block.end(),
        expected_md5_block.begin(), expected_md5_block.end()
    );

    fs::remove_all(test_dir);
}

BOOST_AUTO_TEST_CASE(MD5NoPaddingTest)
{
    fs::path test_dir = fs::temp_directory_path() / "bayan_test_temp_md5_nopad";
    fs::create_directories(test_dir);
    fs::path file_path = test_dir / "test_md5_nopad.txt";

    std::string content = "Hello, world!";
    create_test_file(file_path, content); 

    size_t block_size = content.size(); 
    
    BlockHashProvider provider(file_path, block_size, HashAlgorithm::MD5);

    std::vector<char> block(content.begin(), content.end());
    HashValue expected_md5 = calculate_boost_md5(block);
    
    HashValue actual_md5 = provider.get_hash_of_block(0);

    BOOST_CHECK_EQUAL(actual_md5.size(), 16); 
    
    std::stringstream expected_hex, actual_hex;
    for(int i = 0; i < 16; i++)
    {
        expected_hex << std::hex << std::setw(2) << std::setfill('0') 
                    << static_cast<int>(expected_md5[i]);
        actual_hex << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(actual_md5[i]);
    }
    
    BOOST_TEST_MESSAGE("Expected MD5 (no padding): " << expected_hex.str());
    BOOST_TEST_MESSAGE("Actual MD5 (no padding): " << actual_hex.str());
    
    BOOST_CHECK_EQUAL_COLLECTIONS(
        actual_md5.begin(), actual_md5.end(),
        expected_md5.begin(), expected_md5.end()
    );

    fs::remove_all(test_dir);
}

BOOST_AUTO_TEST_SUITE_END()