#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Subject.h"
#include "Command.h"

class BulkProcessor : public Subject
{
private:
    size_t bulkSize;
    bool isDynamicBlock;
    int nestingLevel;
    std::vector<Command> commands;

public:
    BulkProcessor(size_t bulkSize)
		: bulkSize{bulkSize}
		, isDynamicBlock{false}
		, nestingLevel{0}
	{}

public:
    ~BulkProcessor()
	{
        if (isDynamicBlock)
		{
            commands.clear();
        }
		else
		{
            flushBulk();
        }
    }

private:
    void flushBulk()
	{
        if (!commands.empty())
		{
            notify(commands);
            commands.clear();
        }
    }

public:
    void ProcessCommand(const Command& cmd)
	{
        if (cmd.getCmd() == "{")
		{
            if (!isDynamicBlock)
			{
                flushBulk();
                isDynamicBlock = true;
            }
            nestingLevel++;
        }
        else if (cmd.getCmd() == "}")
		{
            if (isDynamicBlock && --nestingLevel == 0)
			{
                flushBulk();
                isDynamicBlock = false;
            }
        }
        else
		{
            commands.push_back(cmd);
            if (!isDynamicBlock && commands.size() >= bulkSize)
			{
                flushBulk();
            }
        }
    }
};