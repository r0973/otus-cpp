#!/bin/bash

# Загружаем общие функции
source ./test_funcs.sh

# Настройки для этого скрипта
TEST_ROOT="${DEFAULT_TEST_ROOT_PREFIX}_crc32"
BIN_PATH="$DEFAULT_BIN_PATH"
LIB_PATH="$DEFAULT_LIB_PATH"

# Проверяем бинарник
if ! check_binary "$BIN_PATH"; then
    exit 1
fi

# Проверяем библиотеку
if ! check_lib "$LIB_PATH"; then
    exit 1
fi

# Основной тест
echo "=== ТЕСТ CRC32 АЛГОРИТМА ==="

create_test_structure "$TEST_ROOT"

echo ""
echo "--- Тест 1: Рекурсивный поиск дубликатов CRC32 ---"
OUTPUT=$(run_bayan "$BIN_PATH" -i "$TEST_ROOT" -e "$TEST_ROOT/exclude_dir")
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ] && \
   output_contains "$OUTPUT" "file1.txt" "file3.txt" && \
   output_not_contains "$OUTPUT" "file2.txt" "excluded.txt"; then
    echo "✓ Тест 1: УСПЕШНО"
else
    echo "✗ Тест 1: ПРОВАЛ"
    echo "Вывод программы:"
    echo "$OUTPUT"
fi

echo ""
echo "--- Тест 2: Уровень сканирования 0 ---"
OUTPUT=$(run_bayan "$BIN_PATH" -i "$TEST_ROOT" -l 0)

if output_not_contains "$OUTPUT" "Found 1 groups"; then
    echo "✓ Тест 2: УСПЕШНО"
else
    echo "✗ Тест 2: ПРОВАЛ"
fi

cleanup_test "$TEST_ROOT"
echo "=== ТЕСТ CRC32 ЗАВЕРШЕН ==="