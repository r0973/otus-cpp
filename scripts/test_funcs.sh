#!/bin/bash

# Общие функции для тестирования bayan

# Пути и настройки по умолчанию
if [ -n "$TEST_SCRIPTS_DIR" ]; then
    SCRIPT_DIR="$TEST_SCRIPTS_DIR"
else
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
fi

DEFAULT_BUILD_DIR="$SCRIPT_DIR"
DEFAULT_BIN_PATH="$DEFAULT_BUILD_DIR/bayan"
DEFAULT_TEST_ROOT_PREFIX="$(dirname "$SCRIPT_DIR")/test_root"
DEFAULT_LIB_PATH="$(dirname "$SCRIPT_DIR")/lib"

# Проверка наличия бинарника
check_binary() {
    local bin_path="${1:-$DEFAULT_BIN_PATH}"
    
    if [ ! -f "$bin_path" ]; then
        echo "Ошибка: Бинарный файл не найден по пути $bin_path"
        echo "Убедитесь, что вы выполнили:"
        echo "  cmake --build build --target install"
        echo "  или make install"
        return 1
    fi
    return 0
}

# Проверка наличия бинарника
check_lib() {
    local lib_path="${1:-$DEFAULT_LIB_PATH}"
    
    if [ ! -d "$lib_path" ]; then
        echo "Ошибка: Директория с библиотеками не найдена по пути $lib_path"
        echo "Абсолютный путь: $(realpath "$lib_path" 2>/dev/null || echo "не существует")"
        return 1
    fi
    
    # Проверяем наличие конкретной библиотеки
    if [ ! -f "$lib_path/libbayan_logic.so" ]; then
        echo "Ошибка: libbayan_logic.so не найдена в $lib_path"
        return 1
    fi

    export LD_LIBRARY_PATH="$lib_path:$LD_LIBRARY_PATH"
    # echo "Добавлен путь к библиотекам: $lib_path"
    return 0

}

# Создание тестовой структуры
create_test_structure() {
    local test_root="$1"
    
    echo "Создание тестовой структуры в $test_root..."
    
    # Очистка
    rm -rf "$test_root"
    
    # Создание директорий
    mkdir -p "$test_root/dirA"
    mkdir -p "$test_root/dirB"
    mkdir -p "$test_root/exclude_dir"
    
    # Создание файлов
    echo "Content of file 1 and file 3" > "$test_root/dirA/file1.txt"
    echo "Content of file 2" > "$test_root/dirA/file2.txt"
    cp "$test_root/dirA/file1.txt" "$test_root/dirB/file3.txt"
    echo "Excluded file content" > "$test_root/exclude_dir/excluded.txt"
    echo "Root unique file" > "$test_root/root_unique.log"
    
    echo "Тестовая структура создана"
}

create_nested_structure() {
    local test_root="$1"
    
    # Очистка
    rm -rf "$test_root" 2>/dev/null || true
    
    # Создаем многоуровневую структуру каталогов
    mkdir -p "$test_root/level1/dirA/subdir1"
    mkdir -p "$test_root/level1/dirB/subdir2"
    mkdir -p "$test_root/level2/dirC"
    mkdir -p "$test_root/exclude_me/inside"
    
    # Файлы на разных уровнях с одинаковым содержимым (дубликаты)
    echo "Identical file content" > "$test_root/level1/dirA/file1.txt"
    echo "Identical file content" > "$test_root/level1/dirB/subdir2/file2.txt"
    echo "Identical file content" > "$test_root/level2/file3.txt"
    
    # Файлы с разным содержимым
    echo "Different content 1" > "$test_root/level1/dirA/unique1.txt"
    echo "Different content 2" > "$test_root/level1/dirB/unique2.txt"
    
    # Файлы в исключенной директории
    echo "Should be excluded" > "$test_root/exclude_me/excluded1.txt"
    echo "Should also be excluded" > "$test_root/exclude_me/inside/excluded2.txt"
}

# Очистка тестовой среды
cleanup_test() {
    local test_root="$1"
    
    if [ -n "$test_root" ]; then
        echo "Очистка тестовой среды: $test_root"
        rm -rf "$test_root"
    fi
}

# Запуск программы и проверка выхода
run_bayan() {
    local bin_path="$1"
    shift
    local args="$@"
    
    echo "Запуск: $bin_path $args"
    "$bin_path" $args
    return $?
}

# Проверка, что вывод содержит строки
output_contains() {
    local output="$1"
    shift
    local strings=("$@")
    
    for str in "${strings[@]}"; do
        if ! echo "$output" | grep -q "$str"; then
            return 1
        fi
    done
    return 0
}

# Проверка, что вывод НЕ содержит строки
output_not_contains() {
    local output="$1"
    shift
    local strings=("$@")
    
    for str in "${strings[@]}"; do
        if echo "$output" | grep -q "$str"; then
            return 1
        fi
    done
    return 0
}