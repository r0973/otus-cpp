#pragma once

#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include "Observer.h"
#include "Command.h"

class FileLogger : public Observer
{
public:
	~FileLogger() override = default;

protected:
    virtual std::string LogName()
	{
        auto now = std::chrono::system_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "bulk" << now_time << milliseconds.count() << ".log";
        return ss.str();
    }

public:
    void update(const std::vector<Command>& commands) override
	{
        std::string filename = LogName();
		std::ofstream ofs{filename};
		for (const auto& cmd : commands)
		{
			ofs << cmd.getCmd() << std::endl;
		}
    }
};