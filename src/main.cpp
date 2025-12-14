#include "lib_version.h"

#include <iostream>
#include <optional>
#include "NoSQLDataBase.h"

using namespace nosqldb;

int main() {
    std::cout << "Starting NoSQL DB Example (Version " << version() << ")" << std::endl;
    
    NoSQLDataBase storage;

    storage.Put<int>("apples", 10);
    storage.Put<int>("apples", 20);

    std::optional<int> apples_last = storage.Get<int>("apples");
    if (apples_last.has_value()) {
        std::cout << "Apples (last version): " << apples_last.value() << std::endl;
    }

    // Получаем все версии (указываем тип явно)
    auto versions = storage.GetVersions<int>("apples");
    std::cout << "Apples (all versions):" << std::endl;
    for (const auto& value : versions) {
        std::cout << "  - " << value << std::endl;
    }

    // Проверяем существование ключа
    std::cout << "Contains 'apples'? " << (storage.Exists("apples") ? "Yes" : "No") << std::endl;

    return 0;
}