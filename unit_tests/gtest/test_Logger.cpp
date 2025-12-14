#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <thread>

#include "Logger.h"

using namespace nosqldb;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testLogFile = "test_log_" + std::to_string(std::time(nullptr)) + ".log";
        Logger::GetInstance().Initialize(testLogFile, LogLevel::DEBUG);
        Logger::GetInstance().Info("Clearing log file for a fresh test run.");
        std::ofstream ofs(testLogFile, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }
    
    void TearDown() override {
        // Сброс логгера для следующего теста
        Logger::GetInstance().Reset();
        // Очищаем файл после теста
        std::remove(testLogFile.c_str());
    }
    
    std::string testLogFile;
};

TEST_F(LoggerTest, LogLevelsWithFormatting) {
    Logger& logger = Logger::GetInstance();
    
    // Эти сообщения должны записаться
    logger.Debug("Debug message %d", 1);
    logger.Info("Info message %s", "test");
    logger.Warn("Warn message %f", 3.14);
    logger.Error("Error message");
    logger.Fatal("Fatal message");
    
    // Проверяем, что файл создан и содержит записи
    std::ifstream file(testLogFile);
    ASSERT_TRUE(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_NE(content.find("DEBUG"), std::string::npos);
    EXPECT_NE(content.find("INFO"), std::string::npos);
    EXPECT_NE(content.find("WARN"), std::string::npos);
    EXPECT_NE(content.find("ERROR"), std::string::npos);
    EXPECT_NE(content.find("FATAL"), std::string::npos);
}

TEST_F(LoggerTest, LogLevelsWithoutFormatting) {
    Logger& logger = Logger::GetInstance();
    
    // Тестируем простые методы без форматирования
    logger.Debug("Simple debug message");
    logger.Info("Simple info message");
    logger.Warn("Simple warn message");
    
    std::ifstream file(testLogFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_NE(content.find("Simple debug message"), std::string::npos);
    EXPECT_NE(content.find("Simple info message"), std::string::npos);
    EXPECT_NE(content.find("Simple warn message"), std::string::npos);
}

TEST_F(LoggerTest, LogLevelFiltering) {
    Logger& logger = Logger::GetInstance();
    logger.SetLogLevel(LogLevel::WARN);
    
    logger.Debug("This should not appear %d", 42);
    logger.Info("This should not appear");
    logger.Warn("This should appear");
    logger.Error("This should appear %s", "error");
    
    std::ifstream file(testLogFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_EQ(content.find("DEBUG"), std::string::npos);
    EXPECT_EQ(content.find("INFO"), std::string::npos);
    EXPECT_NE(content.find("WARN"), std::string::npos);
    EXPECT_NE(content.find("ERROR"), std::string::npos);
}

TEST_F(LoggerTest, ThreadSafety) {
    Logger& logger = Logger::GetInstance();
    logger.SetLogLevel(LogLevel::INFO);
    
    constexpr int NUM_THREADS = 10;
    constexpr int MESSAGES_PER_THREAD = 100;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < NUM_THREADS; t++) {
        threads.emplace_back([&logger, t]() {
            for (int i = 0; i < MESSAGES_PER_THREAD; i++) {
                logger.Info("Thread %d, message %d", t, i);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что все сообщения записаны без падений
    std::ifstream file(testLogFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Должно быть как минимум NUM_THREADS * MESSAGES_PER_THREAD строк
    size_t lineCount = std::count(content.begin(), content.end(), '\n');
    EXPECT_GE(lineCount, NUM_THREADS * MESSAGES_PER_THREAD);
}

TEST_F(LoggerTest, ConsoleOutput) {
    // Тестируем вывод в консоль (без файла)
    Logger& logger = Logger::GetInstance();
    
    // Переинициализируем для консольного вывода
    logger.Initialize("", LogLevel::INFO);
    
    // Эти сообщения должны вывестись в консоль
    logger.Info("Console info message");
    logger.Warn("Console warn message");
    
    // Не должно быть исключений
    EXPECT_NO_THROW(logger.Error("Console error message"));
}