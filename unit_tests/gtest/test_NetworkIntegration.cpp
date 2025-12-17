#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <filesystem>
#include "nosqldb/network/NoSQLServiceImpl.h"
#include "nosqldb/network/NoSQLClient.h"

namespace fs = std::filesystem;

class NetworkIntegrationTest : public ::testing::Test {
protected:
    const std::string server_address = "127.0.0.1:50052"; // Отдельный порт для тестов
    const std::string test_root = "./test_network_root";
    std::unique_ptr<grpc::Server> server;
    std::thread server_thread;

    void SetUp() override {
        if (fs::exists(test_root)) fs::remove_all(test_root);
        fs::create_directories(test_root);

        // Запуск сервера в отдельном потоке
        server_thread = std::thread([this]() {
            nosqldb::NoSQLServiceImpl service(test_root);
            grpc::ServerBuilder builder;
            builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
            builder.RegisterService(&service);
            server = builder.BuildAndStart();
            server->Wait();
        });

        // Даем серверу немного времени на запуск
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void TearDown() override {
        if (server) {
            server->Shutdown();
        }
        if (server_thread.joinable()) {
            server_thread.join();
        }
        fs::remove_all(test_root);
    }
};

// Тест 1: Проверка базового цикла Put-Get по сети
TEST_F(NetworkIntegrationTest, FullNetworkCycle) {
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    nosqldb::NoSQLClient client(channel);

    std::string db = "net_test_db";
    std::string key = "hello_key";
    std::string value = "Hello, Network!";

    // 1. Put
    ASSERT_TRUE(client.Put(db, key, nosqldb::AnyData(value)));

    // 2. Get
    auto res = client.Get(db, key);
    ASSERT_TRUE(res.has_value());
    
    std::string restored_val;
    res->TryGet(restored_val);
    EXPECT_EQ(restored_val, value);
}

// Тест 2: Проверка персистентности (диск + сеть)
TEST_F(NetworkIntegrationTest, PersistenceOverNetwork) {
    {
        auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
        nosqldb::NoSQLClient client(channel);
        client.Put("persist_db", "k1", nosqldb::AnyData(12345));
        client.SyncAll(); // Принудительно на диск
    }

    // Имитируем "перезагрузку" сервера: выключаем старый, включаем новый
    server->Shutdown();
    if (server_thread.joinable()) server_thread.join();

    // Запускаем сервер заново на тех же данных
    server_thread = std::thread([this]() {
        nosqldb::NoSQLServiceImpl service(test_root);
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        server = builder.BuildAndStart();
        server->Wait();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Проверяем, что данные доступны
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    nosqldb::NoSQLClient client(channel);
    auto res = client.Get("persist_db", "k1");
    
    ASSERT_TRUE(res.has_value());
    int val = 0;
    res->TryGet(val);
    EXPECT_EQ(val, 12345);
}
