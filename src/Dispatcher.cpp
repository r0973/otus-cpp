#include "Dispatcher.h"
#include "Command.h"
#include "ConsoleLogger.h"
#include "ThreadSafeFileLogger.h"
#include "ThreadSafeQueue.h"
#include <iostream>

Dispatcher::Dispatcher()
{
    logQueue = std::make_shared<ThreadSafeQueue<std::vector<Command>>>();
    fileQueue = std::make_shared<ThreadSafeQueue<std::vector<Command>>>();

    logThread = std::thread(&Dispatcher::logThreadFunc, this);
    fileThread1 = std::thread(&Dispatcher::fileThreadFunc1, this);
    fileThread2 = std::thread(&Dispatcher::fileThreadFunc2, this);
}

Dispatcher::~Dispatcher()
{
    stop();
    if (logThread.joinable())
    {
        logThread.join();
    }
    if (fileThread1.joinable())
    {
        fileThread1.join();
    }
    if (fileThread2.joinable())
    {
        fileThread2.join();
    }
}

void Dispatcher::stop()
{
    logQueue->push({});
    fileQueue->push({});
    fileQueue->push({});
}

void Dispatcher::processCommands(const std::vector<Command>& commands)
{
    if (commands.empty())
        return;
    logQueue->push(commands);
    fileQueue->push(commands);
}

void Dispatcher::logThreadFunc()
{
    ConsoleLogger logger;
    while (true)
    {
        std::vector<Command> commands;
        logQueue->wait_and_pop(commands);
        if (commands.empty())
            break;
        logger.update(commands);
    }
}

void Dispatcher::fileThreadFunc1()
{
    ThreadSafeFileLogger logger{1};
    while (true)
    {
        std::vector<Command> commands;
        fileQueue->wait_and_pop(commands);
        if (commands.empty())
            break;
        logger.update(commands);
    }
}

void Dispatcher::fileThreadFunc2()
{
    ThreadSafeFileLogger logger{2};
    while (true)
    {
        std::vector<Command> commands;
        fileQueue->wait_and_pop(commands);
        if (commands.empty())
            break;
        logger.update(commands);
    }
}
