// examples/protobuf_example.cpp (или обновите user_storage_example.cpp)

#include <iostream>
#include <memory>
#include "NoSQLDataBase.h"
#include "protos/user.pb.h"    // Подключаем сгенерированные заголовки
#include "protos/product.pb.h"

using namespace nosqldb;
using namespace nosqldb::data; 

int main()
{
    std::cout << "--- Запуск примера с Google Protobuf структурами ---" << std::endl;

    NoSQLDataBase db;

    // 1. Создание объектов User и Product с помощью методов Protobuf
    User user1;
    user1.set_id(1);
    user1.set_name("Alice Protobuf");
    user1.set_email("alice@proto.com");
    user1.set_age(30);
    user1.set_city("Zurich");

    Product prod1;
    prod1.set_product_id(101);
    prod1.set_name("Laptop");
    prod1.set_price(999.99);

    // 2. Сохранение в базу данных (AnyData автоматически оборачивает их)
    db.Put("user:1", user1);
    db.Put("product:101", prod1);

    std::cout << "   Сохранено: " << user1.name() << std::endl;
    std::cout << "   Сохранено: " << prod1.name() << std::endl;


    // 3. Извлечение данных и проверка
    auto retrievedUser = db.Get<User>("user:1");
    if (retrievedUser.has_value()) {
        std::cout << "   Извлечено имя пользователя: " << retrievedUser->name() << std::endl;
        assert(retrievedUser->email() == "alice@proto.com");
    }

    auto retrievedProduct = db.Get<Product>("product:101");
    if (retrievedProduct.has_value()) {
        std::cout << "   Извлечено название продукта: " << retrievedProduct->name() << std::endl;
        assert(std::abs(retrievedProduct->price() - 999.99) < 0.001);
    }
    
    // В этом примере объекты User и Product существуют в памяти в виде Protobuf объектов.
    // Если бы мы включили сохранение на диск (Уровень 2), то при вызове db.Put/Get 
    // автоматически использовался бы BinarySerializer<User/Product> и методы Protobuf 
    // для конвертации в/из бинарного формата на диске.

    std::cout << "--- Пример Protobuf завершен успешно ---" << std::endl;
}
