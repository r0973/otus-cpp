#pragma once

#include <fstream>
#include <string>
#include <filesystem>
#include "AnyData.h"
#include "protos/nosql_service.pb.h"

namespace fs = std::filesystem;

namespace nosqldb {

class WalManager {
public:
    explicit WalManager(const std::string& directory) {
        wal_path_ = (std::filesystem::path(directory) / "wal.log").string();
        log_file_.open(wal_path_, std::ios::app | std::ios::binary);
    }

    void LogPut(const std::string& key, const proto::AnyDataProto& value) {
        std::lock_guard lock(mutex_);
        if (!log_file_.is_open()) return;
        
        // ВАЖНО: Добавляем тип операции 1 для Put
        uint8_t type = 1;
        log_file_.write(reinterpret_cast<const char*>(&type), sizeof(type));

        proto::PutRequest entry;
        entry.set_key(key);
        *entry.mutable_value() = value;

        uint32_t size = entry.ByteSizeLong();
        log_file_.write(reinterpret_cast<const char*>(&size), sizeof(size));
        entry.SerializeToOstream(&log_file_);
        log_file_.flush();
    }

    void LogDelete(const std::string& key) {
        std::lock_guard lock(mutex_);
        if (!log_file_.is_open()) return;
    
        uint8_t type = 2; // Тип операции 2 для Delete
        log_file_.write(reinterpret_cast<const char*>(&type), sizeof(type));

        proto::DeleteRequest entry; 
        entry.set_key(key);

        uint32_t size = entry.ByteSizeLong();
        log_file_.write(reinterpret_cast<const char*>(&size), sizeof(size));
        entry.SerializeToOstream(&log_file_);
        log_file_.flush(); 
    }

    void Clear() {
        std::lock_guard lock(mutex_);
        if (log_file_.is_open()) {
            log_file_.close();
        }
        if (std::filesystem::exists(wal_path_)) {
            std::filesystem::remove(wal_path_);
        }
        log_file_.open(wal_path_, std::ios::binary | std::ios::app);
    }

    // Добавим метод для восстановления (проигрывания лога)
    template<typename PutH, typename DelH>
    void Recover(PutH&& put_handler, DelH&& del_handler) {
        std::lock_guard lock(mutex_);
        if (log_file_.is_open()) log_file_.close();

        std::ifstream in(wal_path_, std::ios::binary);
        if (!in) return; // Если файла нет, восстанавливать нечего

        while (in.peek() != EOF) {
            uint8_t type = 0;
            in.read(reinterpret_cast<char*>(&type), sizeof(type));

            uint32_t size = 0;
            in.read(reinterpret_cast<char*>(&size), sizeof(size));
            
            std::vector<char> buffer(size);
            in.read(buffer.data(), size);

            if (type == 1) { // Put
                proto::PutRequest req;
                if (req.ParseFromArray(buffer.data(), size)) {
                    put_handler(req.key(), AnyData::FromProto(req.value()));
                }
            } else if (type == 2) { // Delete
                proto::DeleteRequest req;
                if (req.ParseFromArray(buffer.data(), size)) {
                    del_handler(req.key()); // Вызываем обработчик удаления
                }
            }
        }
        in.close();
        // Снова открываем для записи в режиме append
        log_file_.open(wal_path_, std::ios::binary | std::ios::app);
    }

private:
    std::ofstream log_file_;
    std::string wal_path_;
    std::mutex mutex_;
};

} // namespace nosqldb