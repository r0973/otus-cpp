#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <iostream>
#include <sstream>
#include <utility>

namespace nosqldb {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& GetInstance();
    
    void Initialize(const std::string& filename = "", LogLevel level = LogLevel::INFO);
    void SetLogLevel(LogLevel level);
    void Reset(); // сброс
    // Основной метод логирования
    void Log(LogLevel level, const std::string& message);
    
    // Методы с форматированием через stringstream
    template<typename... Args>
    void Log(LogLevel level, Args&&... args);
    
    template<typename... Args>
    void Debug(Args&&... args);
    
    template<typename... Args>
    void Info(Args&&... args);
    
    template<typename... Args>
    void Warn(Args&&... args);
    
    template<typename... Args>
    void Error(Args&&... args);
    
    template<typename... Args>
    void Fatal(Args&&... args);
    
    // Простые методы без форматирования
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warn(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
private:
    Logger() = default;
    ~Logger();
    
private:
    void WriteToFileOrConsole(const std::string& formattedMessage);
    std::string FormatLogMessage(LogLevel level, const std::string& message);
    std::string GetCurrentTime();
    std::string LogLevelToString(LogLevel level);
    
    // Вспомогательный метод для форматирования
    template<typename... Args>
    static std::string FormatMessage(Args&&... args);
private:    
    std::unique_ptr<std::ofstream> fileStream_;
    LogLevel currentLevel_ = LogLevel::INFO;
    std::mutex mutex_;
    bool initialized_ = false;
};

// Реализация шаблонных методов с форматированием
template<typename... Args>
std::string Logger::FormatMessage(Args&&... args) {
    std::ostringstream stream;
    (stream << ... << std::forward<Args>(args));
    return stream.str();
}

template<typename... Args>
void Logger::Log(LogLevel level, Args&&... args) {
    if (level < currentLevel_ || !initialized_) return;
    
    std::string message = FormatMessage(std::forward<Args>(args)...);
    std::string formattedMessage = FormatLogMessage(level, message);
    
    std::lock_guard lock(mutex_);
    if (fileStream_) {
        *fileStream_ << formattedMessage << std::endl;
        fileStream_->flush();
    } else {
        std::cout << formattedMessage << std::endl;
    }
}

template<typename... Args>
void Logger::Debug(Args&&... args) {
    Log(LogLevel::DEBUG, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Info(Args&&... args) {
    Log(LogLevel::INFO, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Warn(Args&&... args) {
    Log(LogLevel::WARN, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Error(Args&&... args) {
    Log(LogLevel::ERROR, std::forward<Args>(args)...);
}

template<typename... Args>
void Logger::Fatal(Args&&... args) {
    Log(LogLevel::FATAL, std::forward<Args>(args)...);
}

} // namespace nosqldb