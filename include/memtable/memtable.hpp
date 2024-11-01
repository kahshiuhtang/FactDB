#ifndef MEMTABLE_FACTDB_HPP
#define MEMTABLE_FACTDB_HPP

#include <unordered_map>
#include <string>
#include <memory>

#include "internal/skiplist.hpp"
#include "sstable/sstable.hpp"

namespace factdb{
class Memtable {
public:
    void insert(std::string partition_key, std::string cluster_key, std::string value);
    bool update(std::string partition_key, std::string cluster_key, std::string value);
    void remove();
    factdb::SSTable flush_to_sstable();
private:
    std::unordered_map<std::string, std::shared_ptr<factdb::SkipList<std::string, std::string>>> skiplist_map_; //map<parititon_key, skiplist<cluster_key, value>>
};
}
#endif