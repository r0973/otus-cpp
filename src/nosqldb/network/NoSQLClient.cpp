#include "NoSQLClient.h"

namespace nosqldb {

NoSQLClient::NoSQLClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(::nosqldb::proto::NoSQLService::NewStub(channel)) {}

bool NoSQLClient::Put(const std::string& db_name, const std::string& key, const AnyData& data) {
    ::nosqldb::proto::PutRequest request;
    request.set_db_name(db_name);
    request.set_key(key);
    *request.mutable_value() = data.ToProto();

    grpc::ClientContext context;
    ::google::protobuf::Empty response;
    grpc::Status status = stub_->Put(&context, request, &response);
    
    return status.ok();
}

std::optional<AnyData> NoSQLClient::Get(const std::string& db_name, const std::string& key) {
    ::nosqldb::proto::GetRequest request;
    request.set_db_name(db_name);
    request.set_key(key);

    ::nosqldb::proto::GetResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->Get(&context, request, &response);

    if (status.ok() && response.found()) {
        return AnyData::FromProto(response.value());
    }
    return std::nullopt;
}

bool NoSQLClient::Delete(const std::string& db_name, const std::string& key) {
    ::nosqldb::proto::DeleteRequest request;
    request.set_db_name(db_name);
    request.set_key(key);

    grpc::ClientContext context;
    ::google::protobuf::Empty response;
    grpc::Status status = stub_->Delete(&context, request, &response);
    return status.ok();
}

std::vector<std::string> NoSQLClient::ListStorages() {
    ::google::protobuf::Empty request;
    ::nosqldb::proto::ListStoragesResponse response;
    grpc::ClientContext context;

    std::vector<std::string> result;
    grpc::Status status = stub_->ListStorages(&context, request, &response);
    if (status.ok()) {
        for (const auto& name : response.names()) {
            result.push_back(name);
        }
    }
    return result;
}

bool NoSQLClient::SyncAll() {
    ::google::protobuf::Empty request;
    ::google::protobuf::Empty response;
    grpc::ClientContext context;
    return stub_->SyncAll(&context, request, &response).ok();
}

} // namespace nosqldb
