#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include "NoSQLDataBase.h"

using namespace nosqldb;

class ComparisonBenchmark {
public:
    struct ComparisonPoint {
        int dataSize;
        double nosqldbTimeMs;
        double stdMapTimeMs;
        double speedup;
    };
    
    static void RunComparison() {
        std::cout << "Comparing NoSQLDB with std::unordered_map...\n\n";
        
        std::vector<int> sizes = {100, 500, 1000, 5000, 10000, 50000};
        std::vector<ComparisonPoint> results;
        
        for (int size : sizes) {
            // Тест NoSQLDB
            auto nosqldbTime = TestNoSQLDB(size);
            
            // Тест std::unordered_map
            auto stdMapTime = TestStdMap(size);
            
            double speedup = stdMapTime > 0 ? nosqldbTime / stdMapTime : 0;
            
            results.push_back({size, nosqldbTime, stdMapTime, speedup});
            
            std::cout << "Size: " << size 
                      << " | NoSQLDB: " << nosqldbTime << " ms"
                      << " | std::map: " << stdMapTime << " ms"
                      << " | Speedup: " << (speedup > 1 ? "slower" : "faster")
                      << " by factor: " << speedup << "\n";
        }
        
        // Сохраняем результаты в файл
        SaveResultsToCSV(results);
        
        std::cout << "\nResults saved to benchmark_results.csv\n";
    }
    
private:
    static double TestNoSQLDB(int size) {
        StorageConfig config;
        config.lruCacheCapacity = size;
        config.enableIndexing = false;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        NoSQLDataBase db(config);
        
        // Запись
        for (int i = 0; i < size; i++) {
            db.Put("key_" + std::to_string(i), i);
        }
        
        // Чтение
        int sum = 0;
        for (int i = 0; i < size; i++) {
            auto val = db.Get<int>("key_" + std::to_string(i));
            if (val) sum += *val;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    static double TestStdMap(int size) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::unordered_map<std::string, int> map;
        map.reserve(size);
        
        // Запись
        for (int i = 0; i < size; i++) {
            map["key_" + std::to_string(i)] = i;
        }
        
        // Чтение
        int sum = 0;
        for (int i = 0; i < size; i++) {
            auto it = map.find("key_" + std::to_string(i));
            if (it != map.end()) {
                sum += it->second;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    static void SaveResultsToCSV(const std::vector<ComparisonPoint>& results) {
        std::ofstream file("benchmark_results.csv");
        file << "DataSize,NoSQLDB_Time_ms,StdMap_Time_ms,Speedup_Factor\n";
        
        for (const auto& result : results) {
            file << result.dataSize << ","
                 << result.nosqldbTimeMs << ","
                 << result.stdMapTimeMs << ","
                 << result.speedup << "\n";
        }
        
        file.close();
    }
};

int main()
{
    ComparisonBenchmark::RunComparison();
    return 0;
}