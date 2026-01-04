#include "async.h"
#include "AsyncLoggerAdaptor.h"
#include "Dispatcher.h"
#include "BulkProcessor.h"
#include <unordered_map>
#include <memory>
#include <mutex>

struct ContextState
{
    size_t bulk_size;
    std::shared_ptr<BulkProcessor> processor;
    std::shared_ptr<Dispatcher> dispatcher;
    std::shared_ptr<AsyncLoggerAdapter> adapter;
    std::mutex mutex;
    
    ContextState(size_t size) : bulk_size(size)
    {
        dispatcher = std::make_shared<Dispatcher>();
        processor = std::make_shared<BulkProcessor>(bulk_size);
        adapter = std::make_shared<AsyncLoggerAdapter>(dispatcher);
        processor->attach(adapter);
    }
};

static std::unordered_map<void*, std::unique_ptr<ContextState>> contexts;
static std::mutex contexts_mutex;

void* connect(size_t bulkSize)
{
    auto ctx = std::make_unique<ContextState>(bulkSize);
    void* handle = ctx.get();
    
    std::lock_guard<std::mutex> lock(contexts_mutex);
    contexts[handle] = std::move(ctx);
    
    return handle;
}

void receive(void* handle, const char* data, size_t size)
{
    ContextState* ctx = nullptr;
    
    {
        std::lock_guard<std::mutex> lock(contexts_mutex);
        auto it = contexts.find(handle);
        if (it == contexts.end()) return;
        ctx = it->second.get();
    }
    
    if (!ctx) return;
    
    std::string cmd_str(data, size);
    if (cmd_str.empty()) return;
    
    Command cmd{cmd_str};
    
    std::lock_guard<std::mutex> lock(ctx->mutex);
    ctx->processor->ProcessCommand(cmd);
}

void disconnect(void* handle)
{
    std::unique_ptr<ContextState> ctx;
    
    {
        std::lock_guard<std::mutex> lock(contexts_mutex);
        auto it = contexts.find(handle);
        if (it == contexts.end()) return;
        ctx = std::move(it->second);
        contexts.erase(it);
    }
    
    if (ctx) {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        ctx->processor->Finish();
    }
}