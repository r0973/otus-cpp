#include "async.h"
#include "AsyncLoggerAdaptor.h"
#include "Dispatcher.h"
#include "BulkProcessor.h"
#include <mutex>
#include <memory>

// Глобальные объекты для смешивания статических команд
static std::shared_ptr<Dispatcher> global_dispatcher = std::make_shared<Dispatcher>();
static std::shared_ptr<BulkProcessor> global_processor = nullptr;
static std::mutex global_mtx;

struct ContextState
{
    size_t bulk_size;
    std::shared_ptr<BulkProcessor> local_processor; // Для динамических блоков
    bool is_dynamic = false;
    int nesting_level = 0;

    ContextState(size_t size)
    : bulk_size(size)
    {}
};

void* connect(size_t bulkSize)
{
    std::lock_guard<std::mutex> lock(global_mtx);
    if (!global_processor)
    {
        global_processor = std::make_shared<BulkProcessor>(bulkSize);
        auto adapter = std::make_shared<AsyncLoggerAdapter>(global_dispatcher);
        global_processor->attach(adapter);
    }
    return new ContextState(bulkSize);
}

void receive(void* context, const char* buffer, size_t size)
{
    auto state = static_cast<ContextState*>(context);
    std::string cmd_str(buffer, size);
    if (cmd_str.empty())
        return;

    Command cmd(cmd_str);

    if (cmd.isBlockStart())
    {
        if (!state->is_dynamic)
        {
            state->is_dynamic = true;
            // Создаем локальный процессор для динамического блока
            state->local_processor = std::make_shared<BulkProcessor>(state->bulk_size);
            state->local_processor->attach(std::make_shared<AsyncLoggerAdapter>(global_dispatcher));
        }
        state->nesting_level++;
    }

    if (state->is_dynamic)
    {
        state->local_processor->ProcessCommand(cmd);
        if (cmd.isBlockEnd())
        {
            state->nesting_level--;
            if (state->nesting_level == 0)
            {
                state->is_dynamic = false;
                state->local_processor->Finish();
                state->local_processor.reset();
            }
        }
    }
    else
    {
        // Статический режим: используем ОБЩИЙ процессор с мьютексом
        std::lock_guard<std::mutex> lock(global_mtx);
        global_processor->ProcessCommand(cmd);
    }
}

void disconnect(void* context)
{
    auto state = static_cast<ContextState*>(context);
    if (state->is_dynamic)
    {
        // Динамические блоки при разрыве выбрасываются (по заданию 7)
    }
    else
    {
        // Проверяем, нужно ли сбросить статический блок (если это был последний клиент)
        // В рамках данного задания статический блок сбросится по заполнению или по завершению сервера
    }
    delete state;
}
