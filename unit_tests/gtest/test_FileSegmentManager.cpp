#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "FileSegmentManager.h"
#include "AnyData.h"

using namespace nosqldb;
namespace fs = std::filesystem;

class FileSegmentManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = "./test_segment_manager";
        fs::create_directories(test_dir_);
        manager_ = std::make_unique<FileSegmentManager>(test_dir_, 3); // 3 записи на сегмент
    }

    void TearDown() override {
        manager_.reset();
        fs::remove_all(test_dir_);
    }

    std::string test_dir_;
    std::unique_ptr<FileSegmentManager> manager_;
};

TEST_F(FileSegmentManagerTest, SaveAndLoadRoundTrip) {
    // 1. Подготовка тестовых данных
    std::unordered_map<std::string, AnyData> test_data = {
        {"key1", AnyData(42)},
        {"key2", AnyData(std::string("hello"))},
        {"key3", AnyData(3.14)},
        {"key4", AnyData(std::string(""))}, // Пустая строка
        {"key5", AnyData(-100)},            // Отрицательное число
    };

    // 2. Сохранение данных
    manager_->Save(test_data);

    // 3. Загрузка данных
    std::unordered_map<std::string, AnyData> loaded_data;
    manager_->Load(loaded_data);

    // 4. Проверка корректности
    ASSERT_EQ(loaded_data.size(), test_data.size());
    ASSERT_EQ(loaded_data["key1"].Get<int>(), 42);
    ASSERT_EQ(loaded_data["key2"].Get<std::string>(), "hello");
    ASSERT_DOUBLE_EQ(loaded_data["key3"].Get<double>(), 3.14);
    ASSERT_EQ(loaded_data["key4"].Get<std::string>(), "");
    ASSERT_EQ(loaded_data["key5"].Get<int>(), -100);
}

TEST_F(FileSegmentManagerTest, SaveAtomicityOnFailure) {
    std::unordered_map<std::string, AnyData> initial_data = {{"k1", AnyData(1)}};
    manager_->Save(initial_data);

    // Имитируем сбой записи: изменяем путь до директории
    fs::rename(test_dir_, test_dir_ + "_moved");

    std::unordered_map<std::string, AnyData> new_data = {{"k2", AnyData(2)}, {"k3", AnyData(3)}};
    
    // Ожидаем, что Save выбросит исключение
    EXPECT_THROW(manager_->Save(new_data), std::exception);

    // 4. Восстанавливаем окружение
    fs::rename(test_dir_ + "_moved", test_dir_);

    std::unordered_map<std::string, AnyData> loaded_data;
    manager_->Load(loaded_data);

    // Атомарность: если Save не прошел целиком, должны остаться ТОЛЬКО старые данные
    ASSERT_EQ(loaded_data.size(), 1);
    for (const auto& [key, value] : initial_data) {
        ASSERT_TRUE(loaded_data.count(key));
        EXPECT_EQ(loaded_data[key], value);
    }
}

TEST_F(FileSegmentManagerTest, SaveMultipleSegments) {
    // 1. Подготовка
    std::unordered_map<std::string, AnyData> test_data;
    const int entries_count = 10;
    const int expected_segments = 4; // 10 / 3 = 3.33 -> 4 файла

    for (int i = 0; i < entries_count; ++i) {
        test_data["key" + std::to_string(i)] = AnyData(i);
    }

    // 2. Сохранение данных
    manager_->Save(test_data);

    // 3. Проверка, что создано несколько файлов сегментов
    int segment_count = 0;
    for (const auto& entry : fs::directory_iterator(test_dir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".db") {
            segment_count++;
        }
    }
    ASSERT_EQ(segment_count, expected_segments);

    // 4. Загрузка и проверка данных
    std::unordered_map<std::string, AnyData> loaded_data;
    manager_->Load(loaded_data);
    for (int i = 0; i < entries_count; ++i) {
        std::string key = "key" + std::to_string(i);
        ASSERT_EQ(loaded_data[key], AnyData{i}) << "Mismatch at " << key;
    }
}

