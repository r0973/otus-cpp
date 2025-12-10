#!/bin/bash

# Загружаем общие функции
source ./test_funcs.sh

# Настройки для этого скрипта
TEST_ROOT="${DEFAULT_TEST_ROOT_PREFIX}_nested"
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

echo "=== ТЕСТ ВЛОЖЕННОЙ СТРУКТУРЫ ==="
echo ""

# Создаем структуру
create_nested_structure "$TEST_ROOT"

echo "--- Запускаем поиск с CRC32:"
OUTPUT_CRC=$(run_bayan "$BIN_PATH" \
    -i "$TEST_ROOT/level1" \
    -i "$TEST_ROOT/level2" \
    -e "$TEST_ROOT/exclude_me" \
    --hash crc32)

# echo "$OUTPUT_CRC"
if output_not_contains "$OUTPUT_CRC" "Found groups"; then
    echo "$OUTPUT_CRC"
    echo "✓ Тест 3: УСПЕШНО"
else
    echo "✗ Тест 3: ПРОВАЛ"
fi

echo ""
echo "Запускаем поиск с MD5:"
OUTPUT_MD5=$(run_bayan "$BIN_PATH" \
    -i "$TEST_ROOT/level1" \
    -i "$TEST_ROOT/level2" \
    -e "$TEST_ROOT/exclude_me" \
    --hash md5)

# echo "$OUTPUT_MD5"
if output_not_contains "$OUTPUT_MD5" "Found groups"; then
    echo "$OUTPUT_MD5"
    echo "✓ Тест 4: УСПЕШНО"
else
    echo "✗ Тест 4: ПРОВАЛ"
fi

# Очистка
cleanup_test "$TEST_ROOT"

echo ""
echo "=== ТЕСТ ЗАВЕРШЕН ==="