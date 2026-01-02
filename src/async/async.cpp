#include "async.h"
#include "AsyncLoggerAdaptor.h"
#include "Dispatcher.h"
#include "BulkProcessor.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>

struct ContextState
{
    std::shared_ptr<BulkProcessor> processor; 
    std::shared_ptr<Dispatcher> dispatcher;
    std::shared_ptr<AsyncLoggerAdapter> adapter; 
};

std::unordered_map<void*, ContextState> contexts;
std::mutex globalContextMutex;

void* connect(size_t bulkSize)
{
    auto dispatcher = std::make_shared<Dispatcher>();
    auto processor = std::make_shared<BulkProcessor>(bulkSize); 
    auto adapter = std::make_shared<AsyncLoggerAdapter>(dispatcher);

    processor->attach(adapter);

    auto* context = new ContextState{processor, dispatcher, adapter};
    std::lock_guard<std::mutex> lockg{globalContextMutex};
    contexts[context] = *context;

    return context;
}

void receive(void* context, const char* buffer, size_t size)
{
    std::string line{buffer, size};
    std::lock_guard<std::mutex> lock{globalContextMutex};
    
    auto it = contexts.find(context);
    if (it != contexts.end())
    {
        it->second.processor->ProcessCommand(Command{line});
    }
}

void disconnect(void* context)
{
    std::lock_guard<std::mutex> lock{globalContextMutex};

    auto it = contexts.find(context);
    if (it != contexts.end())
    {
        it->second.processor->Finish();
        it->second.dispatcher->stop(); 
        delete static_cast<ContextState*>(context);
        contexts.erase(it);
    }
}