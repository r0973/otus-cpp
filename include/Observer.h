#pragma once

#include <vector>
#include <memory>
#include "Command.h"

class Observer
{
public:
    virtual ~Observer() = default;
public:
    virtual void update(const std::vector<Command>& commands) = 0;
};