#include <iostream>
#include <thread>
#include "NoSQLClient.h"

// Для правильной работы данного примера необходимо сначала в отдельном терминале запустить
// запустить сервер ./bin/nosqlserver

int main() {
    // 1. Создаем канал связи
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    nosqldb::NoSQLClient client(channel);

    std::string db = "test_network_db";

    // 2. Пишем данные
    std::cout << "Sending data to server..." << std::endl;
    client.Put(db, "msg", nosqldb::AnyData(std::string("Hello from Network Client!")));
    client.Put(db, "count", nosqldb::AnyData(100));

    // 2.1. Синхронизируем для сброса на диск
    std::cout << "Syncing all data to disk..." << std::endl;
    client.SyncAll();

    // 3. пауза перед чтением
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 3. Читаем данные
    auto res = client.Get(db, "msg");
	if (res) {
    std::string val;
    if (res->TryGet(val)) {
        std::cout << "Received string: " << val << std::endl;
    } else {
        std::cout << "Data found, but type is not string!" << std::endl;
    }
	} else {
    	std::cout << "Key 'msg' not found on server!" << std::endl;
	}

    // 4. Список баз
    auto dbs = client.ListStorages();
    std::cout << "Available storages on server:" << std::endl;
    for (const auto& name : dbs) std::cout << " - " << name << std::endl;

    return 0;
}
