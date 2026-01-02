#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;

public:
    void push(T value)
	{
        std::lock_guard<std::mutex> lock{mutex};
        queue.push(std::move(value));
        condition.notify_one();
    }

public:
    bool try_pop(T& value)
	{
        std::lock_guard<std::mutex> lock{mutex};
        if (queue.empty())
		{
            return false;
        }
        value = std::move(queue.front());
        queue.pop();
        return true;
    }

public:
    void wait_and_pop(T& value)
	{
        std::unique_lock<std::mutex> lock{mutex};
        condition.wait(lock, [this] { return !queue.empty(); });
        value = std::move(queue.front());
        queue.pop();
    }
};
