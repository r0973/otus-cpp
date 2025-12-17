#include <iostream>
#include <thread>
#include <vector>
#include <filesystem>
#include "NoSQLClient.h"

namespace fs = std::filesystem;
using namespace nosqldb;

// Глобальный счетчик для уникальности ключей между разными потоками
void client_stress_task(int client_id, const std::string& db_name, int start_idx, int count) {
    auto channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    NoSQLClient client(channel);

    std::cout << "[Client " << client_id << "] Writing to " << db_name 
              << " (Keys: " << start_idx << " to " << start_idx + count << ")" << std::endl;

    for (int i = start_idx; i < start_idx + count; ++i) {
        std::string key = "user_" + std::to_string(i);
        std::string value = "Data_from_client_" + std::to_string(client_id);
        
        client.Put(db_name, key, AnyData(value));
        
        // Раз в 50 записей просим сервер сбросить данные на диск
        if (i % 50 == 0) client.SyncAll();
    }
    
    client.SyncAll();
}

int main() {
    std::string root_data = "./demo_data";
    if (fs::exists(root_data)) fs::remove_all(root_data);

    std::cout << "=== Multi-Client Persistence & Segmentation Demo ===" << std::endl;
    std::cout << "Scenario: 6 Clients -> 3 Databases (2 clients per DB)" << std::endl;

    std::vector<std::thread> workers;

    // База 1: billing_db (Клиенты 1 и 2 пишут по 150 записей = 300 всего)
    workers.emplace_back(client_stress_task, 1, "billing_db", 0, 150);
    workers.emplace_back(client_stress_task, 2, "billing_db", 150, 150);

    // База 2: orders_db (Клиенты 3 и 4 пишут по 100 записей = 200 всего)
    workers.emplace_back(client_stress_task, 3, "orders_db", 0, 100);
    workers.emplace_back(client_stress_task, 4, "orders_db", 100, 100);

    // База 3: logs_db (Клиенты 5 и 6 пишут по 50 записей = 100 всего)
    workers.emplace_back(client_stress_task, 5, "logs_db", 0, 50);
    workers.emplace_back(client_stress_task, 6, "logs_db", 50, 50);

    for (auto& t : workers)
		t.join();

	
	std::cout << "\n=== Верификация данных (Чтение разными клиентами) ===" << std::endl;
    auto channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    NoSQLClient verifier(channel);

	// Проверяем выборочные ключи из разных баз
    struct Check { std::string db; std::string key; std::string expected; };
    std::vector<Check> checks = {
        {"billing_db", "user_0",   "Data_from_client_1"}, // Первый ключ клиента 1
        {"billing_db", "user_299", "Data_from_client_2"}, // Последний ключ клиента 2
        {"orders_db",  "user_99",  "Data_from_client_3"}, // Граничный ключ
        {"logs_db",    "user_50",  "Data_from_client_6"}  // Данные из логов
    };

    for (const auto& c : checks) {
        auto res = verifier.Get(c.db, c.key);
        if (res) {
            std::string val;
            res->TryGet(val);
            std::cout << "[Verify] DB: " << c.db << " Key: " << c.key 
                      << (val == c.expected ? " -> SUCCESS" : " -> FAILED") << std::endl;
        } else {
            std::cout << "[Verify] DB: " << c.db << " Key: " << c.key << " -> NOT FOUND!" << std::endl;
        }
    }

    std::cout << "\n=== Проверка структуры сегментов на диске ===" << std::endl;
    std::cout << "(При maxSegmentSize = 100)" << std::endl;

    for (const auto& db_dir : fs::directory_iterator(root_data)) {
        if (db_dir.is_directory()) {
            std::cout << "\nDatabase: " << db_dir.path().filename() << std::endl;
            for (const auto& file : fs::directory_iterator(db_dir.path())) {
                std::cout << "  |- " << file.path().filename() 
                          << " (" << fs::file_size(file.path()) << " bytes)" << std::endl;
            }
        }
    }

    return 0;
}