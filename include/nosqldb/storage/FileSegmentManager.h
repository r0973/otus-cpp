#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "nosqldb/core/AnyData.h"

namespace nosqldb {

class FileSegmentManager {
public:
    // base_path — папка, где лежат файлы (например, "./data_users/")
    // max_entries_per_segment — через сколько ключей создавать новый файл
    FileSegmentManager(const std::string& base_path, size_t max_entries_per_segment);

    // Сохраняет текущий слепок данных в сегменты (seg_0.db, seg_1.db...)
    void Save(const std::unordered_map<std::string, AnyData>& data);

    // Находит все файлы в base_path и загружает их в out_data
    void Load(std::unordered_map<std::string, AnyData>& out_data);

private:
    std::string base_path_;
    size_t max_entries_;

    std::string get_segment_path(size_t index) const;
};

} // namespace nosqldb
