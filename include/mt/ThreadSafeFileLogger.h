// ThreadSafeFileLogger.h
#pragma once

#include "FileLogger.h"
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <atomic>
#include <thread>
#include <unistd.h>  // для getpid()

class ThreadSafeFileLogger : public FileLogger
{
private:
    int thread_id;
    static std::atomic<int> global_counter;

public:
    ThreadSafeFileLogger(int id) : thread_id(id) {}
    ~ThreadSafeFileLogger() override = default;

protected:
    std::string LogName() override
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        );
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch() % std::chrono::seconds(1)
        );
        
        std::stringstream ss;
        ss << "bulk_"
           << getpid() << "_"                    // PID процесса
           << thread_id << "_"                   // ID потока (1 или 2)
           << ms.count() << "_"                  // миллисекунды
           << micros.count() << "_"              // микросекунды
           << global_counter++ << ".log";        // глобальный счетчик
        
        return ss.str();
    }

public:
    void update(const std::vector<Command>& commands) override
    {
        if (commands.empty()) return;
        
        std::string filename = LogName();  // НОВОЕ имя для каждого блока!
        std::ofstream ofs{filename};
        
        for (const auto& cmd : commands)
        {
            if (cmd.shouldBeProcessed())  // только обычные команды
            {
                ofs << cmd.getCmd() << std::endl;
            }
        }
    }
};
