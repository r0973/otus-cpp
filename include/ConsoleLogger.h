#pragma once

#include <iostream>
#include "Observer.h"
#include "Command.h"

class ConsoleLogger : public Observer
{
public:
	~ConsoleLogger() override = default;

public:
    void update(const std::vector<Command>& commands) override
	{
        std::cout << "bulk: ";
        for (size_t i = 0; i < commands.size(); ++i)
		{
            std::cout << commands[i].getCmd();
            if (i != commands.size() - 1)
			{
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
};