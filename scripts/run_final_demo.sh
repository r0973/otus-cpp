#!/bin/bash

# Пути к бинарным файлам (относительно корня проекта)
SERVER_BIN="./bin/nosqlserver"
DEMO_BIN="./bin/final_demo_segmentation_example"
DB_PATH="./demo_data"
PORT="50051"

# Проверка наличия файлов
if [ ! -f "$SERVER_BIN" ] || [ ! -f "$DEMO_BIN" ]; then
    echo "Ошибка: Бинарные файлы не найдены. Сначала выполните сборку (make)."
    exit 1
fi

echo "--- Подготовка: Очистка старых данных ---"
rm -rf "$DB_PATH"

echo "--- 1. Запуск NoSQL сервера на порту $PORT ---"
# Запускаем сервер в фоне (&) и сохраняем его PID
$SERVER_BIN $PORT $DB_PATH &
SERVER_PID=$!

# Даем серверу 2 секунды, чтобы подняться и открыть порт
sleep 2

echo "--- 2. Запуск демонстрационного стресс-теста ---"
# Запускаем клиентскую часть (демо)
$DEMO_BIN

# Сохраняем код возврата демо
RESULT=$?

echo -e "\n--- 3. Завершение работы ---"
# Убиваем процесс сервера
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

if [ $RESULT -eq 0 ]; then
    echo "Демонстрация завершена УСПЕШНО."
else
    echo "Демонстрация завершена с ОШИБКОЙ."
fi

exit $RESULT