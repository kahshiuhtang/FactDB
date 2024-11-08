#include <data/memtable.hpp>
#include <internal/consts.hpp>

void factdb::Memtable::insert(std::string partition_key, std::string cluster_key, std::string value){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->insert(cluster_key, value);
    } else {
        auto partition_skiplist = std::make_shared<factdb::SkipList<std::string, std::string>>(MAX_SKIPLIST_HEIGHT, NEW_SKIPLIST_LAYER_PROB);
        partition_skiplist->insert(cluster_key, value);
        skiplist_map_[partition_key] = partition_skiplist; 
    }
}
bool factdb::Memtable::update(std::string partition_key, std::string cluster_key, std::string value){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->update(cluster_key, value);
        return true;
    }
    return false;
}
void factdb::Memtable::remove(){

}
std::shared_ptr<factdb::SSTable> factdb::Memtable::flush_to_sstable(std::string &table_id){
    std::shared_ptr<factdb::SSTable> sstable = std::make_shared<factdb::SSTable>();
    std::ofstream datafile(table_id, std::ios::binary | std::ios::out);
    if (!datafile.is_open()) {
        throw std::runtime_error("Failed to open SSTable file for writing");
    }
    for (const auto& partition_entry : skiplist_map_) {
        const std::string& partition_key = partition_entry.first;
        auto partition_skiplist = partition_entry.second;
    }
    skiplist_map_.clear();
    return sstable;
}