#pragma once

#pragma once
#include "FileLogger.h"
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>

class ThreadSafeFileLogger : public FileLogger
{
private:
    std::string filename;
    int thread_id; // ID потока

public:
    ThreadSafeFileLogger(int id) : thread_id(id)
    {
        filename = LogName(); 
    }
    
    ~ThreadSafeFileLogger() override = default;

protected:
    std::string LogName() override
    {
        std::stringstream ss;
        std::string baseName = FileLogger::LogName(); 
        size_t dotPos = baseName.find_last_of('.');
        if (dotPos == std::string::npos)
        {
            ss << baseName << "_thread" << thread_id << ".log";
        }
        else
        {
            ss << baseName.substr(0, dotPos);
            ss << "_thread" << thread_id;
            ss << baseName.substr(dotPos);
        }
        
        return ss.str();
    }

public:
    void update(const std::vector<Command>& commands) override
    {
        std::ofstream ofs{filename, std::ios_base::app}; 

        for (const auto& cmd : commands)
        {
            ofs << cmd.getCmd() << std::endl;
        }
    }
};