#include <chrono>
#include <iomanip>
#include <filesystem>
#include "Logger.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace nosqldb {

Logger::~Logger() {
    if (fileStream_) {
        fileStream_->flush();
        fileStream_->close();
    }
}

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

void Logger::Initialize(const std::string& filename, LogLevel level) {
    std::lock_guard lock(mutex_);
    
    if (initialized_) {
        return;
    }
    
    currentLevel_ = level;
    
    if (!filename.empty()) {
        // Создаем директорию если нужно
        std::filesystem::path path(filename);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        fileStream_ = std::make_unique<std::ofstream>(filename, std::ios::app);
        if (!fileStream_->is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            fileStream_.reset();
        }
    }
    
    initialized_ = true;
    WriteToFileOrConsole(FormatLogMessage(
                            LogLevel::INFO,
                            "Logger initialized with level: " + 
                            LogLevelToString(level)));
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard lock(mutex_);
    currentLevel_ = level;
}

void Logger::Reset() {
    std::lock_guard lock(mutex_);
    if (fileStream_) {
        fileStream_->close();
        fileStream_.reset();
    }
    initialized_ = false;
    currentLevel_ = LogLevel::INFO; // Или дефолтный уровень
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level < currentLevel_ || !initialized_) return;
    
    std::string formattedMessage = FormatLogMessage(level, message);
    
    std::lock_guard lock(mutex_);
    WriteToFileOrConsole(formattedMessage);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warn(const std::string& message) {
    Log(LogLevel::WARN, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::FATAL, message);
}

void Logger::WriteToFileOrConsole(const std::string& formattedMessage) {
    if (fileStream_) {
        *fileStream_ << formattedMessage << std::endl;
        fileStream_->flush();
    } else {
        std::cout << formattedMessage << std::endl;
    }
}
    

std::string Logger::FormatLogMessage(LogLevel level, const std::string& message) {
    std::stringstream ss;
    ss << "[" << GetCurrentTime() << "] "
       << "[" << LogLevelToString(level) << "] "
       << message;
    return ss.str();
}

std::string Logger::GetCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    // Получаем миллисекунды
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

} // namespace nosqldb