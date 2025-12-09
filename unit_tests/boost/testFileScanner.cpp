#include <fstream>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "FileScanner.h"
#include "Config.h"

namespace fs = boost::filesystem;

void create_test_file_fs(const fs::path& path, const std::string& content)
{
    std::ofstream ofs(path.string(), std::ios::binary);
    ofs << content;
    ofs.close();
}

struct FsFixture
{
    FsFixture() : test_dir(fs::temp_directory_path() / "bayan_fs_tests")
	{
        cleanup();
        fs::create_directories(test_dir);
        fs::create_directories(test_dir / "subdir");
        fs::create_directories(test_dir / "exclude_dir");

        create_test_file_fs(test_dir / "fileA.txt", "same content");
        create_test_file_fs(test_dir / "subdir/fileB.txt", "same content"); // Duplicate of fileA.txt
        create_test_file_fs(test_dir / "fileC.log", "unique log content");
        create_test_file_fs(test_dir / "exclude_dir/fileD.txt", "excluded content");
        create_test_file_fs(test_dir / "small_file.txt", "s"); // 1 byte
    }
    ~FsFixture()
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

BOOST_FIXTURE_TEST_SUITE(FileScannerTests, FsFixture)

BOOST_AUTO_TEST_CASE(ScannerFilteringTest)
{
    Configuration config;
    config.include_dirs.push_back(test_dir);
    config.exclude_dirs.push_back(test_dir / "exclude_dir");
    config.recursion_level = -1; // Infinite recursion
    config.min_file_size = 2; // Exclude small_file.txt
    config.file_masks.push_back("*.txt"); // Include only txt

    FileScanner scanner(config);
    FileGroupsBySize groups = scanner.scan();

    BOOST_CHECK_EQUAL(groups.size(), 1); 

    BOOST_CHECK_EQUAL(groups.begin()->second.size(), 2);
    bool foundA = false, foundB = false;
    for(const auto& p : groups.begin()->second)
	{
        if (p.filename() == "fileA.txt") foundA = true;
        if (p.filename() == "fileB.txt") foundB = true;
    }
    BOOST_CHECK(foundA && foundB);
}

BOOST_AUTO_TEST_SUITE_END()
