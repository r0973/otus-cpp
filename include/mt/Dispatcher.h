#pragma once

#include "Command.h"
#include "ConsoleLogger.h"
#include "ThreadSafeFileLogger.h"
#include "ThreadSafeQueue.h"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Dispatcher
{
private:
    std::atomic<bool> stopped{false};
	
public:
    Dispatcher();
    ~Dispatcher();
    void stop();
    void processCommands(const std::vector<Command>& commands);

private:
    std::shared_ptr<ThreadSafeQueue<std::vector<Command>>> logQueue;
    std::shared_ptr<ThreadSafeQueue<std::vector<Command>>> fileQueue;

    std::thread logThread;
    std::thread fileThread1;
    std::thread fileThread2;
    
    void logThreadFunc();
    void fileThreadFunc1();
    void fileThreadFunc2();
};