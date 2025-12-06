#pragma once
#include "Observer.h"
#include "Dispatcher.h"
#include <memory>
#include <vector>

class AsyncLoggerAdapter : public Observer
{
private:
    std::shared_ptr<Dispatcher> dispatcher;

public:
    AsyncLoggerAdapter(std::shared_ptr<Dispatcher> disp)
        : dispatcher(std::move(disp))
    {}
    ~AsyncLoggerAdapter() override = default;

public:
    void update(const std::vector<Command>& commands) override
    {
        dispatcher->processCommands(commands);
    }
};
