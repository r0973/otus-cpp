#!/bin/bash

# Загружаем общие функции
source ./test_funcs.sh

# Настройки для этого скрипта
TEST_ROOT="${DEFAULT_TEST_ROOT_PREFIX}_md5"
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

# Дополнительные файлы для MD5 теста
create_md5_test_structure() {
    local test_root="$1"
    
    # Используем базовую структуру
    create_test_structure "$test_root"
    
    # Добавляем специфичные для MD5 файлы
    echo "MD5 test file identical" > "$test_root/dirA/md5_file1.bin"
    echo "MD5 test file identical" > "$test_root/dirB/md5_file2.bin"
    
    # Файл с тем же размером, но другим содержимым
    echo "Same size different content" | dd bs=1 count=24 2>/dev/null > "$test_root/different.bin"
    
    echo "Дополнительные MD5 тестовые файлы созданы"
}

echo "=== ТЕСТ MD5 АЛГОРИТМА ==="

create_md5_test_structure "$TEST_ROOT"

echo ""
echo "--- Тест 1: Базовый поиск с MD5 ---"
OUTPUT=$(run_bayan "$BIN_PATH" -i "$TEST_ROOT" -e "$TEST_ROOT/exclude_dir" --hash md5)
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ] && \
   output_contains "$OUTPUT" "md5_file1.bin" "md5_file2.bin" && \
   output_not_contains "$OUTPUT" "different.bin"; then
    echo "✓ Тест 1: УСПЕШНО (MD5 нашел дубликаты)"
else
    echo "✗ Тест 1: ПРОВАЛ"
fi

echo ""
echo "--- Тест 2: Сравнение CRC32 и MD5 ---"
CRC_OUTPUT=$(run_bayan "$BIN_PATH" -i "$TEST_ROOT/dirA" -i "$TEST_ROOT/dirB" --hash crc32)
MD5_OUTPUT=$(run_bayan "$BIN_PATH" -i "$TEST_ROOT/dirA" -i "$TEST_ROOT/dirB" --hash md5)

# Считаем количество найденных файлов в выводе
CRC_COUNT=$(echo "$CRC_OUTPUT" | grep -c "\.txt\|\.bin" || true)
MD5_COUNT=$(echo "$MD5_OUTPUT" | grep -c "\.txt\|\.bin" || true)

if [ "$CRC_COUNT" -eq "$MD5_COUNT" ]; then
    echo "✓ Тест 2: УСПЕШНО (CRC32 и MD5 дали одинаковое количество: $CRC_COUNT)"
else
    echo "✗ Тест 2: ПРОВАЛ (CRC32: $CRC_COUNT, MD5: $MD5_COUNT)"
fi

echo ""
echo "--- Тест 3: MD5 с разными размерами блоков ---"
run_bayan "$BIN_PATH" -i "$TEST_ROOT" --hash md5 --block-size 64 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Тест 3.1: УСПЕШНО (block-size=64)"
else
    echo "✗ Тест 3.1: ПРОВАЛ"
fi

run_bayan "$BIN_PATH" -i "$TEST_ROOT" --hash md5 --block-size 1024 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Тест 3.2: УСПЕШНО (block-size=1024)"
else
    echo "✗ Тест 3.2: ПРОВАЛ"
fi

cleanup_test "$TEST_ROOT"
echo "=== ТЕСТ MD5 ЗАВЕРШЕН ==="