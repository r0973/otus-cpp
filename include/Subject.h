#pragma once
#include <vector>
#include <memory>
#include "Observer.h"

class Subject
{
private:
    std::vector<std::shared_ptr<Observer>> observers;

public:
    virtual ~Subject() = default;

public:
	void attach(std::shared_ptr<Observer> observer)
	{
        observers.push_back(observer);
    }

public:
    void notify(const std::vector<Command>& commands)
	{
        for (auto& observer : observers)
		{
            observer->update(commands);
        }
    }

};