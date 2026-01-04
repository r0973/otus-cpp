#include "ThreadSafeFileLogger.h"


// Статическая инициализация
std::atomic<int> ThreadSafeFileLogger::global_counter{0};