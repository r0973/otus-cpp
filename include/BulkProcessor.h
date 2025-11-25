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
    explicit BulkProcessor(size_t blockSize_)
		: bulkSize{blockSize_}
		, isDynamicBlock{false}
		, nestingLevel{0}
	{}

public:
	void Finish()
	{
		if (!isDynamicBlock && !commands.empty())
		{
			flushBulk();
		}
	}
public:
    ~BulkProcessor()
	{}

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
		if (cmd.getCmd().empty())
			return;

		if (cmd.getCmd() == "{")
		{
			if (!isDynamicBlock)
			{
				flushBulk();
				isDynamicBlock = true;
			}
			nestingLevel++;
			return;
		}
		else if (cmd.getCmd() == "}")
		{
			if (isDynamicBlock && --nestingLevel == 0)
			{
				flushBulk();
				isDynamicBlock = false;
			}
			return;
		}

		if (cmd.getCmd() != "EOF")
		{
			if (isDynamicBlock)
			{
				commands.push_back(cmd);
			}
			else if (bulkSize > 0)
			{
				commands.push_back(cmd);
				
				if (commands.size() >= bulkSize)
				{
					flushBulk();
				}
			}
		}
	}
};