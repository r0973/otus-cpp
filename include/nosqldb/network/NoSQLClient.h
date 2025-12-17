#pragma once

#include <grpcpp/grpcpp.h>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "nosql_service.grpc.pb.h"
#include "AnyData.h"

namespace nosqldb {

class NoSQLClient {
public:
    // Подключение к серверу (например, "localhost:50051")
    explicit NoSQLClient(std::shared_ptr<grpc::Channel> channel);

    // Удаленное сохранение данных
    bool Put(const std::string& db_name, const std::string& key, const AnyData& data);

    // Удаленное получение данных
    std::optional<AnyData> Get(const std::string& db_name, const std::string& key);

    // Удаление данных
    bool Delete(const std::string& db_name, const std::string& key);

    // Получение списка баз на сервере
    std::vector<std::string> ListStorages();

    // Принудительная синхронизация всех БД на сервере
    bool SyncAll();

private:
    std::unique_ptr<::nosqldb::proto::NoSQLService::Stub> stub_;
};

} // namespace nosqldb
