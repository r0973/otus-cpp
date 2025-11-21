#pragma once

#include <fstream>
#include <ctime>
#include "Observer.h"
#include "Command.h"

class FileLogger : public Observer
{
public:
	~FileLogger() override = default;

public:
    void update(const std::vector<Command>& commands) override
	{
        std::time_t now = std::time(nullptr);
        std::string filename = "bulk" + std::to_string(now) + ".log";
        std::ofstream ofs{filename};
        for (const auto& cmd : commands)
		{
            ofs << cmd.getCmd() << std::endl;
        }
    }
};