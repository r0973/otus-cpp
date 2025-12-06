#pragma once
#include <cstddef>

void* connect(size_t bulkSize);
void receive(void* context, const char* buffer, size_t size);
void disconnect(void* context);