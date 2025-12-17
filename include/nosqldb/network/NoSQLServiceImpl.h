#pragma once

#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h> //!!!
#include "nosql_service.pb.h"       // cначала данные
#include "nosql_service.grpc.pb.h"  // потом сервис

#include "StorageManager.h"
#include "StorageConfig.h"

namespace nosqldb {

class NoSQLServiceImpl final : public nosqldb::proto::NoSQLService::Service {
public:
    explicit NoSQLServiceImpl(const std::string& root_path
                             , const StorageConfig& consfig = StorageConfig{});
    ~NoSQLServiceImpl() override = default; 
    grpc::Status Put([[maybe_unused]] grpc::ServerContext* context, 
                     const nosqldb::proto::PutRequest* request,
                     [[maybe_unused]] google::protobuf::Empty* response) override;

    grpc::Status Get([[maybe_unused]] grpc::ServerContext* context, 
                     const nosqldb::proto::GetRequest* request,
                     nosqldb::proto::GetResponse* response) override;

    grpc::Status ListStorages([[maybe_unused]] grpc::ServerContext* context, 
                              [[maybe_unused]] const google::protobuf::Empty* request,
                              nosqldb::proto::ListStoragesResponse* response) override;
    
    grpc::Status SyncAll([[maybe_unused]] grpc::ServerContext* context, 
                         [[maybe_unused]] const google::protobuf::Empty* request,
                         [[maybe_unused]] google::protobuf::Empty* response) override;

    grpc::Status Delete([[maybe_unused]] grpc::ServerContext* context, 
                         const ::nosqldb::proto::DeleteRequest* request,
                         [[maybe_unused]] ::google::protobuf::Empty* response) override;
private:
    StorageManager manager_;
    StorageConfig config_;
};

} // namespace nosqldb
