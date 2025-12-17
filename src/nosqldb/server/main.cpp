#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "NoSQLServiceImpl.h"

void RunServer(const std::string& db_path, const std::string& port) {
    std::string server_address("0.0.0.0:" + port);
    nosqldb::NoSQLServiceImpl service(db_path);

    grpc::ServerBuilder builder;
    // Слушаем порт без шифрования (Insecure) для курсового проекта
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "NoSQL Server listening on " << server_address << std::endl;
    std::cout << "Databases root path: " << db_path << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    std::string port = "50051";
    std::string db_path = "./server_data";

    if (argc > 1) port = argv[1];
    if (argc > 2) db_path = argv[2];

    RunServer(db_path, port);
    return 0;
}
