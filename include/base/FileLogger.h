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
        using namespace std::chrono;
        auto now = system_clock::now();
        auto now_time = system_clock::to_time_t(now);
        
        // Наносекунды для большей уникальности
        auto nanos = duration_cast<nanoseconds>(
            now.time_since_epoch() % seconds(1)
        );
        
        std::stringstream ss;
        ss << "bulk_" << now_time << "_" << nanos.count() << ".log";
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