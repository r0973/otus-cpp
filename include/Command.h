#pragma once

#include <string>

class Command
{
    std::string cmd;
public:
    Command(std::string cmd_)
	 : cmd{std::move(cmd_)}
	{}
public:
    const std::string& getCmd() const { return cmd; }
    bool empty() const { return cmd.empty(); }
    bool isBlockStart() const { return cmd == "{"; }
    bool isBlockEnd() const { return cmd == "}"; }
    bool isEOF() const { return cmd == "EOF"; }
    bool isControlCommand() const { return isBlockStart() || isBlockEnd(); }
    bool shouldBeProcessed() const { return !empty() && !isEOF() && !isControlCommand(); }
};