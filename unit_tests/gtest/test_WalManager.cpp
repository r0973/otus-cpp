#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "WalManager.h"
#include "AnyData.h"

namespace fs = std::filesystem;
using namespace nosqldb;

class WalManagerTest : public ::testing::Test
{
protected:
    std::string test_dir = "./test_wal_data";

    // Выполняется перед каждым тестом
    void SetUp() override {
        if (fs::exists(test_dir)) fs::remove_all(test_dir);
        fs::create_directories(test_dir);
    }

    // Выполняется после каждого теста
    void TearDown() override {
        fs::remove_all(test_dir);
    }
};

// 1. Тест базовой записи и восстановления (Put)
TEST_F(WalManagerTest, SaveAndRecoverPut)
{
    {
        WalManager wal(test_dir);
        wal.LogPut("key1", AnyData(123).ToProto());
        wal.LogPut("key2", AnyData(std::string("hello")).ToProto());
    } // Здесь файл закрывается

    WalManager wal(test_dir);
    int put_count = 0;
    
    wal.Recover(
        [&](const std::string& key, const AnyData& value) {
            put_count++;
            if (key == "key1") { EXPECT_EQ(value.Get<int>(), 123); }
            if (key == "key2") { EXPECT_EQ(value.Get<std::string>(), "hello"); }
        },
        [](const std::string&) { FAIL() << "Should not have deletes"; }
    );

    EXPECT_EQ(put_count, 2);
}

// 2. Тест записи удаления (Delete)
TEST_F(WalManagerTest, SaveAndRecoverDelete)
{
    {
        WalManager wal(test_dir);
        wal.LogDelete("temporary_key");
    }

    WalManager wal(test_dir);
    bool delete_called = false;

    wal.Recover(
        [](const std::string&, const AnyData&) { FAIL() << "Should not have puts"; },
        [&](const std::string& key) {
            delete_called = true;
            EXPECT_EQ(key, "temporary_key");
        }
    );

    EXPECT_TRUE(delete_called);
}

// 3. Тест очистки лога (Checkpoint)
TEST_F(WalManagerTest, ClearLog)
{
    WalManager wal(test_dir);
    wal.LogPut("key", AnyData(1).ToProto());
    
    std::string wal_file = (fs::path(test_dir) / "wal.log").string();
    EXPECT_GT(fs::file_size(wal_file), 0);

    wal.Clear();

    EXPECT_TRUE(fs::exists(wal_file));
    EXPECT_EQ(fs::file_size(wal_file), 0);
}

// 4. Тест на "смешанный" лог (Put + Delete)
TEST_F(WalManagerTest, MixedOperations)
{
    {
        WalManager wal(test_dir);
        wal.LogPut("k1", AnyData(1.1).ToProto());
        wal.LogDelete("k1");
        wal.LogPut("k1", AnyData(2.2).ToProto());
    }

    WalManager wal(test_dir);
    std::vector<std::string> ops;

    wal.Recover(
        [&](const std::string& k, const AnyData&) { ops.push_back("put_" + k); },
        [&](const std::string& k) { ops.push_back("del_" + k); }
    );

    ASSERT_EQ(ops.size(), 3);
    EXPECT_EQ(ops[0], "put_k1");
    EXPECT_EQ(ops[1], "del_k1");
    EXPECT_EQ(ops[2], "put_k1");
}

// 5. Тест устойчивости к отсутствию файла
TEST_F(WalManagerTest, RecoveryWithNoFile)
{
    WalManager wal(test_dir);
    fs::remove(fs::path(test_dir) / "wal.log");

    EXPECT_NO_THROW({
        wal.Recover(
            [](const std::string&, const AnyData&) {},
            [](const std::string&) {}
        );
    });
}
