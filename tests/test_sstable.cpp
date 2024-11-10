#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "data/memtable.hpp"
#include "internal/consts.hpp"

class MemtableTest : public ::testing::Test {
protected:
    factdb::Memtable memtable;

    std::shared_ptr<factdb::MemtableColumn> create_column(const std::string& value) {
        auto col = std::make_shared<factdb::MemtableColumn>();
        col->set_serialized_val_(value);
        return col;
    }
};

TEST_F(MemtableTest, ConvertObjToRow_ClusterKey) {
    factdb::Memtable memtable;
    std::unordered_map<std::string, std::shared_ptr<factdb::MemtableColumn>> objstate;
    std::string cluster_key = "cluster1";

    // // Create some columns
    objstate["col1"] = create_column("value1");
    objstate["cluster1"] = create_column("cluster_value");

    std::shared_ptr<factdb::Row> row = memtable.convert_obj_to_row_(&objstate, &cluster_key);

    ASSERT_EQ(row->clustering_blocks_.size(), 1);
    auto cb = row->clustering_blocks_[0];
    ASSERT_EQ(cb->clustering_cells_.size(), 1);
    //std::string expectstr1(cb->clustering_cells_[0].get_cell_(0).value_.begin(), cb->clustering_cells_[0].get_cell_(0).value_.end());
    //ASSERT_EQ(expectstr1, "cluster_value");

    ASSERT_EQ(row->cells_.size(), 1);
    //std::string expectstr2(row->cells_[0].get_cell_(0).value_.begin(), row->cells_[0].get_cell_(0).value_.end());
    //ASSERT_EQ(expectstr2, "value1");
}

// TEST_F(MemtableTest, ConvertObjToRow_NoClusterKey) {
//     std::unordered_map<std::string, std::shared_ptr<factdb::MemtableColumn>> objstate;
//     std::string cluster_key = "cluster1";

//     // Create some columns
//     objstate["col1"] = create_column("value1");
//     objstate["col2"] = create_column("value2");

//     // Call the function
//     std::shared_ptr<factdb::Row> row = memtable.convert_obj_to_row_(&objstate, &cluster_key);

//     // Check that no clustering block is created, only regular cells
//     ASSERT_EQ(row->clustering_blocks_.size(), 0);
//     ASSERT_EQ(row->cells_.size(), 2);
//     //ASSERT_EQ(row->cells_[0].value_->value_, "value1");
//     //ASSERT_EQ(row->cells_[1].value_->value_, "value2");
// }