# Поиск файлов-дубликатов по их содержимому

## Описание проекта
Утилита для поиска файлов-дубликатов на основе поблочного хэширования. Проект использует CMake и поддерживает несколько алгоритмов хэширования (CRC32, MD5).

## Структура проекта

Проект организован в соответствии со стандартными практиками C++/CMake. Исходный код имеетследующую структуру:  
```
.
├── CMakeLists.txt
├── Doxyfile
├── README.md
├── googletest/   # Субмодуль Google Test
├── scripts
│   ├── test_crc32.sh
│   ├── test_funcs.sh
│   ├── test_md5.sh
│   ├── test_nested.sh
│   └── test_runner.sh
├── src
│   ├── CMakeLists.txt
│   ├── Comparison.cpp
│   ├── Comparison.h
│   ├── Config.cpp
│   ├── Config.h
│   ├── FileScanner.cpp
│   ├── FileScanner.h
│   ├── HashManager.cpp
│   ├── HashManager.h
│   ├── lib_version.cpp
│   ├── lib_version.h
│   ├── main.cpp
│   └── version.h.in
└── unit_tests
    ├── CMakeLists.txt
    ├── boost
    │   ├── CMakeLists.txt
    │   ├── testComparison.cpp
    │   ├── testFileScanner.cpp
    │   ├── testHashManager.cpp
    │   └── test_version.cpp
    └── gtest
        ├── CMakeLists.txt
        ├── test_main_gtest.cpp
        └── test_version.cpp
```

## Сборка проекта с использованием CMake

Для сборки проекта вам понадобится компилятор C++17 (например, GCC 9+ или Clang 9+), CMake версии 3.12 или выше и библиотеки Boost. 

### 1. Клонирование репозитория (включая субмодули)

`git clone https://github.com/r0973/otus-cpp.git`  
`cd otus-cpp`  
`git checkout feature/duplicate_files_filter`

### 2. Создание каталога сборки

`mkdir build`  
`cd build`  

### 3. Конфигурация проекта

`cmake ..`  

#### Опционально: Вы можете отключить сборку тестов Google Test или Boost, добавив параметры при конфигурации:

`cmake .. -DWITH_GOOGLE_TEST=OFF -DWITH_BOOST_TEST=OFF`  

### 4. Сборка проекта
`cmake --build .`  

### 5. Установка проекта (Копирование артефактов в build/install/)

`cmake --install .`  

### 6. Запуск и тестирование
После установки все необходимые файлы находятся в каталоге `build/install/`.

#### 6.1. Структура проекта после сборки
```
build/install/
├── bin/
│   ├── bayan              # Основная утилита
│   ├── test_runner.sh     # Главный тестовый скрипт
│   ├── test_crc32.sh      # Тест CRC32 алгоритма
│   ├── test_md5.sh        # Тест MD5 алгоритма
│   ├── test_nested.sh     # Тест вложенных директорий
│   └── test_funcs.sh      # Общие функции тестирования
└── lib/
    └── libbayan_logic.so  # Динамическая библиотека
```

#### 6.2. Запуск вручную
Вы можете запустить клиентское приложение `bayan` вручную из каталога `build/install/bin`. Не забудьте установить переменную окружения `LD_LIBRARY_PATH`, чтобы система знала, где искать `libbayan_logic.so`:

`cd build/install/bin/`  
`export LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH`  
`./bayan -i "/path/to/folder" -e "/path/to/exclude_dir" --hash md5`

#### 6.2. Использование автоматических скриптов тестирования
Проект генерирует скрипты `test_runner.sh` в каталоге установки. Этот скрипт тестирование
утилиты и автоматически устанавливает переменную окружения `LD_LIBRARY_PATH`.
Запустите его из каталога `build`:

`cd build`  
`./install/bin/test_runner.sh`  

или любого другого каталога с указанием относительного пути до скрипта.

#### Доступные тесты
```
test_crc32.sh # Тестирование алгоритма CRC32
Рекурсивный поиск дубликатов
Исключение директорий
Разные уровни сканирования

test_md5.sh # Тестирование алгоритма MD5
Сравнение с результатами CRC32
Тестирование граничных случаев

test_nested.sh # Тестирование вложенных директорий
Глубоко вложенные структуры
Символические ссылки
```

#### 6.3. Запуск юнит-тестов
Если вы собирали тесты, вы можете запустить их из каталога сборки:

`cd build/install/bin` 
`./unit_tests_boost`

#### 6.4. Запуск тестов через ctest
`ctest`  