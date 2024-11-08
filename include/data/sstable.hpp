#ifndef SSTABLE_FACTDB_HPP
#define SSTABLE_FACTDB_HPP

#include "data/sstable.hpp"
#include "data/sstable/datafile.hpp"

namespace factdb{
class SSTable{
public:
    SSTable(): file_path_("./data/sstable1.sst"), partitions_() {};
    SSTable(const std::string& file_path): file_path_(file_path), partitions_() {};
    SSTable(const std::string& file_path, const std::vector<std::shared_ptr<factdb::Partition>>& partitions)
    : file_path_(file_path), partitions_(partitions) {};
    bool write_to_file();
    bool read_from_file();
    bool compress();
private:
    std::string file_path_;
    std::vector<std::shared_ptr<factdb::Partition>> partitions_;
};
}
#endif