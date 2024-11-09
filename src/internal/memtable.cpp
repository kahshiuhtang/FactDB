#include <data/memtable.hpp>
#include <internal/consts.hpp>

void factdb::Memtable::insert(std::string partition_key, std::string cluster_key, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>> value){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->insert(cluster_key, value);
    } else {
        auto partition_skiplist = std::make_shared<factdb::SkipList<std::string, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>>>>(MAX_SKIPLIST_HEIGHT, NEW_SKIPLIST_LAYER_PROB);
        partition_skiplist->insert(cluster_key, value);
        skiplist_map_[partition_key] = partition_skiplist; 
    }
}
bool factdb::Memtable::update(std::string partition_key, std::string cluster_key, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>> value){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->update(cluster_key, value);
        return true;
    }
    return false;
}
bool factdb::Memtable::remove(std::string partition_key, std::string cluster_key){
    auto it = skiplist_map_.find(partition_key);
    if (it != skiplist_map_.end()) {
        it->second->remove(cluster_key);
        return true;
    }
    return false;
}
std::shared_ptr<factdb::Row> factdb::Memtable::convert_obj_to_row_(const std::unordered_map<std::string, std::shared_ptr<factdb::MemtableColumn>> *obj, std::string* cluster_key){
    if(obj == nullptr){
        return nullptr;
    }
    std::shared_ptr<factdb::Row> new_row= std::make_shared<factdb::Row>();
    for(auto curr_col: *obj){
        std::string col_name = curr_col.first;
        std::shared_ptr<factdb::MemtableColumn> value = curr_col.second;
        if(col_name == *cluster_key){
            std::shared_ptr<factdb::ClusteringBlock> cb = std::make_shared<factdb::ClusteringBlock>();
            factdb::CellValue curr_cell(col_name.length(), value->get_serialized_val_().length());
            curr_cell.key_ = std::vector<char>(col_name.begin(), col_name.end());
            curr_cell.value_ = std::vector<char>(value->get_serialized_val_().begin(), value->get_serialized_val_().end());
            factdb::SimpleCell simple_cell(curr_cell);
            cb->clustering_cells_.emplace_back(simple_cell);
            new_row->clustering_blocks_.emplace_back(cb);
        }else{
            factdb::CellValue curr_cell(col_name.length(), value->get_serialized_val_().length());
            curr_cell.key_ = std::vector<char>(col_name.begin(), col_name.end());
            curr_cell.value_ = std::vector<char>(value->get_serialized_val_().begin(), value->get_serialized_val_().end());
            factdb::SimpleCell simple_cell(curr_cell);
            new_row->cells_.emplace_back(simple_cell);
        }
    }
    return new_row;
}
std::shared_ptr<factdb::SSTable> factdb::Memtable::flush_to_sstable(std::string &table_id){
    std::shared_ptr<factdb::SSTable> sstable = std::make_shared<factdb::SSTable>();
    std::ofstream datafile(table_id, std::ios::binary | std::ios::out);
    if (!datafile.is_open()) {
        throw std::runtime_error("Failed to open SSTable file for writing");
    }
    for (const auto& partition_entry : skiplist_map_) {
        const std::string& partition_key = partition_entry.first; // partition name
        auto partition_skiplist = partition_entry.second;
        std::shared_ptr<factdb::Partition> partition = std::make_shared<factdb::Partition>();
        partition->header_.key_ = std::vector<char>(partition_key.begin(), partition_key.end());
        bool cluster_key_found = false;
        for (auto it = partition_skiplist->begin(); it != partition_skiplist->end(); ++it){ // going through every partition
            std::string clusterkey = it->key_;
            std::vector<std::shared_ptr<factdb::MemTableValue<std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>>>>> value = it->values_;
            std::unordered_map<std::string, std::shared_ptr<factdb::MemtableColumn>> objstate;
            bool is_clusterkey_found = false;
            for (const auto& memtable_entry : value) { // iterating through all updates to this (partition, cluster key) combo
                const auto memtable_cols = memtable_entry->value_;
                for(const auto& curr_row : *memtable_cols){ // iterating through all groups of rows
                    for(const auto&curr_col : curr_row->getallcols_()){ // going through all cols for this update instance
                        auto col_name = curr_col.first;
                        std::shared_ptr<factdb::MemtableColumn> col_val = curr_col.second;
                        if(col_name == clusterkey) cluster_key_found = true; // checking if we have the cluster key
                        objstate[col_name] = col_val;
                    }
                }
            }
            std::shared_ptr<factdb::Row> curr_row = this->convert_obj_to_row_(&objstate, &clusterkey);
            partition->unfiltereds_.emplace_back(curr_row);
        }
    }
    skiplist_map_.clear();
    return sstable;
}