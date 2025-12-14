#!/bin/bash

# Останавливаем скрипт, если какая-либо команда завершится с ошибкой
set -e

BUILD_DIR="build"

echo "--- Создание директории сборки и переход в нее ---"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "--- Настройка проекта с помощью CMake (включая примеры) ---"
# Убедитесь, что CMakeLists.txt находится на уровень выше
cmake .. -DBUILD_EXAMPLES=ON

echo "--- Сборка проекта (Release конфигурация) ---"
cmake --build . --config Release

echo "--- Сборка завершена. Возвращаемся в корень проекта для запуска ---"
cd ..

echo "--- ЗАПУСК БЕНЧМАРКА: Simple Benchmark ---"
# Путь относительно корня проекта
./build/examples/benchmark_simple

# echo "--- ЗАПУСК ОПЦИОНАЛЬНОГО БЕНЧМАРКА: Comparison Benchmark ---"
# ./build/examples/benchmark_comparison

echo "--- Все бенчмарки выполнены успешно! ---"