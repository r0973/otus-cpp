#!/bin/bash

# test_runner.sh - запускает все тесты с полным выводом

# Определяем директорию, где находится скрипт
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Экспортируем для test_funcs.sh
export TEST_SCRIPTS_DIR="$SCRIPT_DIR"

echo "=== ЗАПУСК ВСЕХ ТЕСТОВ ==="
echo "Директория скриптов: $SCRIPT_DIR"
# echo "Полный путь к test_crc32.sh: $SCRIPT_DIR/test_crc32.sh"
# echo "Существует ли: $([ -f "$SCRIPT_DIR/test_crc32.sh" ] && echo "ДА" || echo "НЕТ")"
echo ""

# Проверяем, существуют ли тестовые скрипты
if [ ! -f "$SCRIPT_DIR/test_crc32.sh" ]; then
    echo "Ошибка: test_crc32.sh не найден"
    exit 1
fi

if [ ! -f "$SCRIPT_DIR/test_md5.sh" ]; then
    echo "Ошибка: test_md5.sh не найден"
    exit 1
fi

if [ ! -f "$SCRIPT_DIR/test_nested.sh" ]; then
    echo "Ошибка: test_nested.sh не найден"
    exit 1
fi

# Проверяем, есть ли test_funcs.sh
if [ ! -f "$SCRIPT_DIR/test_funcs.sh" ]; then
    echo "Предупреждение: test_funcs.sh не найден"
    echo "Тестовые скрипты могут не работать правильно"
fi

# Делаем скрипты исполняемыми (на всякий случай)
chmod +x "$SCRIPT_DIR"/test_*.sh 2>/dev/null

echo "-----------------------------------------"
echo "ЗАПУСК test_crc32.sh:"
echo "-----------------------------------------"
# Запускаем с буферизацией строк для немедленного вывода
(cd "$SCRIPT_DIR" && bash -c './test_crc32.sh') 2>&1

echo ""
echo "-----------------------------------------"
echo "ЗАПУСК test_md5.sh:"
echo "-----------------------------------------"
(cd "$SCRIPT_DIR" && bash -c './test_md5.sh') 2>&1

echo ""
echo "-----------------------------------------"
echo "ЗАПУСК test_nested.sh:"
echo "-----------------------------------------"
(cd "$SCRIPT_DIR" && bash -c './test_nested.sh') 2>&1

echo ""
echo "=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ==="