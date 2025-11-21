#pragma once

#include <string>

class Command
{
public:
    Command(const std::string& cmd_)
		: cmd{cmd_}
	{}
    const std::string& getCmd() const { return cmd; }

private:
    std::string cmd;
};
