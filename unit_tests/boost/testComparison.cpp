#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "Comparison.h"
#include "Config.h"
#include "FileScanner.h"

namespace fs = boost::filesystem;

void create_test_file_comp(const fs::path& path, const std::string& content)
{
    std::ofstream ofs(path.string(), std::ios::binary);
    ofs << content;
    ofs.close();
}

struct CompFixture
{
    CompFixture() : test_dir(fs::temp_directory_path() / "bayan_comp_tests")
	{
        cleanup();
        fs::create_directories(test_dir);
        create_test_file_comp(test_dir / "file1.bin", "content_A");
        create_test_file_comp(test_dir / "file2.bin", "content_A"); // Duplicate
        create_test_file_comp(test_dir / "file3.bin", "content_B"); // Unique content
    }
    ~CompFixture()
	{
        cleanup();
    }
    void cleanup()
	{
        if (fs::exists(test_dir))
		{
            fs::remove_all(test_dir);
        }
    }
    fs::path test_dir;
};


BOOST_FIXTURE_TEST_SUITE(ComparisonTests, CompFixture)

BOOST_AUTO_TEST_CASE(FindDuplicatesTest)
{
    Configuration config;
    config.block_size = 4;
    config.hash_algo = HashAlgorithm::CRC32;

    FileGroupsBySize candidates_by_size;
    candidates_by_size[9].push_back(test_dir / "file1.bin");
    candidates_by_size[9].push_back(test_dir / "file2.bin");
    candidates_by_size[9].push_back(test_dir / "file3.bin");

    DuplicateFinder finder(config);
    DuplicateGroups duplicates = finder.find_duplicates(candidates_by_size);

    BOOST_CHECK_EQUAL(duplicates.size(), 1);
    BOOST_CHECK_EQUAL(duplicates[0].size(), 2);

    bool found1 = false, found2 = false;
    for(const auto& p : duplicates[0]) {
       if (p.filename() == "file1.bin") found1 = true;
       if (p.filename() == "file2.bin") found2 = true;
    }
    BOOST_CHECK(found1 && found2);
}

BOOST_AUTO_TEST_SUITE_END()