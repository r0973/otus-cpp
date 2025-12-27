#include "FileSegmentManager.h"
#include "protos/storage.pb.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace nosqldb {

namespace proto = nosqldb::proto;

FileSegmentManager::FileSegmentManager(const std::string& base_path, size_t max_entries)
    : base_path_(base_path), max_entries_(max_entries) {
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

    // Очищаем старые сегменты перед сохранением (для упрощенной версии)
    for (const auto& entry : fs::directory_iterator(base_path_)) {
        if (fs::is_regular_file(entry) && entry.path().extension() == ".db")
            fs::remove(entry.path());
    }

    for (const auto& [key, value] : data) {
        auto* entry = current_segment.add_entries();
        entry->set_key(key);
        
        // Превращаем AnyData в AnyDataProto
        *entry->mutable_value() = value.ToProto(); 

        entry_count++;

        // Если сегмент заполнен, записываем его и создаем новый
        if (entry_count >= max_entries_) {
            std::ofstream out(get_segment_path(segment_index++), std::ios::binary);
            current_segment.SerializeToOstream(&out);
            current_segment.Clear();
            entry_count = 0;
        }
    }

    // Записываем остатки данных
    if (entry_count > 0) {
        std::ofstream out(get_segment_path(segment_index), std::ios::binary);
        current_segment.SerializeToOstream(&out);
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
