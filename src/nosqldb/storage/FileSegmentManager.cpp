#include "FileSegmentManager.h"
#include "protos/storage.pb.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace nosqldb {

namespace proto = nosqldb::proto;

FileSegmentManager::FileSegmentManager(const std::string& base_path, size_t max_entries)
: base_path_(base_path)
, max_entries_(max_entries) {
    if (!base_path_.empty()) {
        fs::create_directories(base_path_);
    }
}

std::string FileSegmentManager::get_segment_path(size_t index) const {
    return (fs::path(base_path_) / ("segment_" + std::to_string(index) + ".db")).string();
}

void FileSegmentManager::Save(const std::unordered_map<std::string, AnyData>& data) {
    proto::StorageSegment current_segment;
    size_t segment_index = 0;
    size_t entry_count = 0;
    std::vector<std::string> temp_files; // Список временных файлов для очистки при ошибке

    try {
        for (const auto& [key, value] : data) {
            auto* entry = current_segment.add_entries();
            entry->set_key(key);
            *entry->mutable_value() = value.ToProto();
            entry_count++;

            // Если сегмент заполнен, записываем его во временный файл
            if (entry_count >= max_entries_) {
                std::string temp_path = get_segment_path(segment_index) + ".tmp";
                temp_files.push_back(temp_path);

                std::ofstream out(temp_path, std::ios::binary);
                if (!out) {
                    throw std::runtime_error("Failed to open temp segment file: " + temp_path);
                }
                current_segment.SerializeToOstream(&out);
                current_segment.Clear();
                entry_count = 0;
                segment_index++;
            }
        }

        // Записываем остатки данных во временный файл
        if (entry_count > 0) {
            std::string temp_path = get_segment_path(segment_index) + ".tmp";
            temp_files.push_back(temp_path);

            std::ofstream out(temp_path, std::ios::binary);
            if (!out) {
                throw std::runtime_error("Failed to open temp segment file: " + temp_path);
            }
            current_segment.SerializeToOstream(&out);
        }

        // Атомарно заменяем старые файлы на новые
        for (size_t seg_idx = 0; seg_idx < segment_index + (entry_count > 0 ? 1 : 0); ++seg_idx) {
            std::string temp_path = get_segment_path(seg_idx) + ".tmp";
            std::string final_path = get_segment_path(seg_idx);

            // Удаляем старый файл, если существует  
            if (fs::exists(final_path)) {
                fs::remove(final_path);
            }

            // Переименовываем временный файл
            fs::rename(temp_path, final_path);
        }

        // Очищаем список временных файлов (уже переименованы)
        temp_files.clear();

    } catch (const std::exception& e) {
        // В случае ошибки — удаляем временные файлы
        for (const auto& temp_file : temp_files) {
            if (fs::exists(temp_file)) {
                fs::remove(temp_file);
            }
        }
        throw; // Пробрасываем исключение дальше
    }
}

void FileSegmentManager::Load(std::unordered_map<std::string, AnyData>& out_data) {
    if (!fs::exists(base_path_)) return;

    for (const auto& entry : fs::directory_iterator(base_path_)) {
        if (entry.path().extension() == ".db") {
            std::ifstream in(entry.path().string(), std::ios::binary);
            proto::StorageSegment segment;
            if (segment.ParseFromIstream(&in)) {
                for (const auto& proto_entry : segment.entries()) {
                    out_data[proto_entry.key()] = AnyData::FromProto(proto_entry.value());
                }
            }
        }
    }
}

} // namespace nosqldb
