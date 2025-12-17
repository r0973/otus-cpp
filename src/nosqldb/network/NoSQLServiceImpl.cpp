#include <grpcpp/grpcpp.h>
#include "StorageManager.h"
#include "AnyData.h"
#include "NoSQLServiceImpl.h"

namespace nosqldb {

NoSQLServiceImpl::NoSQLServiceImpl(const std::string& root_path, const StorageConfig& config) 
    : manager_(root_path)
	, config_ (config)
{}
    // Реализация RPC Put
grpc::Status NoSQLServiceImpl::Put([[maybe_unused]] grpc::ServerContext* context, 
                                    const nosqldb::proto::PutRequest* request,
                                   [[maybe_unused]] google::protobuf::Empty* response) {
	try {
		// Открываем базу данных по имени из запроса
		auto* db = manager_.OpenStorage(request->db_name(), config_);
		
		// Восстанавливаем AnyData из proto и сохраняем
		db->Put(request->key(), AnyData::FromProto(request->value()));
		
		return grpc::Status::OK;
	} catch (const std::exception& e) {
		return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
	}
}

// Реализация RPC Get
grpc::Status NoSQLServiceImpl::Get([[maybe_unused]] grpc::ServerContext* context, 
                                   const nosqldb::proto::GetRequest* request,
                                   nosqldb::proto::GetResponse* response) {
	try {
		std::cout << "[SERVER] Get request: db=" << request->db_name() 
              << ", key=" << request->key() << std::endl;
		auto* db = manager_.OpenStorage(request->db_name(), StorageConfig{});
		// auto result = db->Get<AnyData>(request->key());
		auto result = db->GetAnyData(request->key()); 

		if (result.has_value()) {
			std::cout << "[SERVER] Key found, converting to proto..." << std::endl;
			response->set_found(true);
			*response->mutable_value() = result->ToProto();
		} else {
			std::cout << "[SERVER] Key NOT found in database" << std::endl;
			response->set_found(false);
		}
		return grpc::Status::OK;
	} catch (const std::exception& e) {
		std::cerr << "[SERVER] Error in Get: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
	}
}

// Реализация RPC ListStorages
grpc::Status NoSQLServiceImpl::ListStorages([[maybe_unused]] grpc::ServerContext* context, 
                                            [[maybe_unused]] const google::protobuf::Empty* request,
                                            nosqldb::proto::ListStoragesResponse* response) {
	auto names = manager_.ListAvailableStorages();
	for (const auto& name : names) {
		response->add_names(name);
	}
	return grpc::Status::OK;
}

grpc::Status NoSQLServiceImpl::SyncAll(
    [[maybe_unused]] grpc::ServerContext* context, 
    [[maybe_unused]] const google::protobuf::Empty* request,
    [[maybe_unused]] google::protobuf::Empty* response) 
{
    try {
        manager_.SyncAll(); // вызов StorageManager
        return grpc::Status::OK;
    } catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

grpc::Status NoSQLServiceImpl::Delete([[maybe_unused]] grpc::ServerContext* context, 
                                     const ::nosqldb::proto::DeleteRequest* request,
                                     [[maybe_unused]] ::google::protobuf::Empty* response) {
    try {
        auto* db = manager_.OpenStorage(request->db_name(), StorageConfig{});
        db->Delete(request->key());
        return grpc::Status::OK;
    } catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }
}

} // namespace nosqldb
