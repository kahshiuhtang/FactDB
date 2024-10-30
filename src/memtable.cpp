#include <memtable/memtable.hpp>
#include <internal/consts.hpp>

void factdb::Memtable::insert(std::string partition_key, std::string cluster_key, std::string value){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->insert(cluster_key, value);
    } else {
        auto partition_skiplist = std::make_shared<factdb::SkipList<std::string, std::string>>(MAX_SKIPLIST_HEIGHT, NEW_SKIPLIST_LAYER_PROB);
        skiplist_map_[partition_key] = partition_skiplist; 
    }
}
void factdb::Memtable::update(){
    
}
void factdb::Memtable::remove(){
    
}
bool factdb::Memtable::flush_to_sstable(){
    
}