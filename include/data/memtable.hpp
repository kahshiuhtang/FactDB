#ifndef MEMTABLE_FACTDB_HPP
#define MEMTABLE_FACTDB_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

#include "internal/skiplist.hpp"
#include "data/sstable.hpp"

namespace factdb{
enum class ColumnType {
    STRING,
    INT,
    BOOL,
    FLOAT,
    ARRAY,
    UNKNOWN
};

class MemtableColumn {
public:
    MemtableColumn() : column_type_(ColumnType::UNKNOWN) {}
    
    MemtableColumn(const std::string& name, ColumnType type, const std::string& value)
        : column_name_(name), column_type_(type), serialized_value_(value) {}

    const std::string& getcolname_() const { return column_name_; }
    void setcolname_(const std::string& name) { column_name_ = name; }

    ColumnType getcoltype_() const { return column_type_; }
    void setcoltype_(ColumnType type) { column_type_ = type; }

    const std::string& get_serialized_val_() const { return serialized_value_; }
    void set_serialized_val_(const std::string& value) { serialized_value_ = value; }

    template <typename T>
    void serialize_col_(const T& value) {
        std::ostringstream oss;
        oss << value;
        serialized_value_ = oss.str();
    }

    template <typename T>
    T deserialize_col_() const {
        std::istringstream iss(serialized_value_);
        T value;
        iss >> value;
        return value;
    }

    void print() const {
        std::cout << "Column Name: " << column_name_ << "\n"
                  << "Column Type: " << convert_type_to_str_() << "\n"
                  << "Serialized Value: " << serialized_value_ << "\n";
    }

private:
    std::string column_name_;
    ColumnType column_type_;
    std::string serialized_value_;

    std::string convert_type_to_str_() const {
        switch (column_type_) {
            case ColumnType::STRING: return "STRING";
            case ColumnType::INT: return "INT";
            case ColumnType::BOOL: return "BOOL";
            case ColumnType::FLOAT: return "FLOAT";
            case ColumnType::ARRAY: return "ARRAY";
            default: return "UNKNOWN";
        }
    }
};
class MemtableRow {
public:
    void addcol_(const std::shared_ptr<MemtableColumn> column) {
        columns_[column->getcolname_()] = column;
    }

    const std::shared_ptr<MemtableColumn> getcol_(const std::string& col_name) const {
        auto it = columns_.find(col_name);
        if (it != columns_.end()) {
            return it->second;
        }
        return nullptr;
    }
    std::unordered_map<std::string, std::shared_ptr<MemtableColumn>> getallcols_(){
        return columns_;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<MemtableColumn>> columns_;
};
class Memtable {
public:
    void insert(std::string partition_key, std::string cluster_key, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>> value);
    bool update(std::string partition_key, std::string cluster_key, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>> value);
    bool remove(std::string partition_key, std::string cluster_key);
    std::shared_ptr<factdb::SSTable> flush_to_sstable(std::string &table_id);
private:
    std::shared_ptr<factdb::Row> convert_obj_to_row_(const std::unordered_map<std::string, std::shared_ptr<factdb::MemtableColumn>> *obj, std::string* cluster_key);
    std::unordered_map<std::string, std::shared_ptr<factdb::SkipList<std::string, std::shared_ptr<std::vector<std::shared_ptr<factdb::MemtableRow>>>>>> skiplist_map_; //map<parititon_key, skiplist<cluster_key, value>>
};
}
#endif