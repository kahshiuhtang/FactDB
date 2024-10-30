#ifndef INDEXFILE_FACTDB_HPP
#define INDEXFILE_FACTDB_HPP

#include <cstdint>
#include <optional>
#include <vector>

class IndexEntry {
    uint32_t length_;
    char *key_;
    uint64_t position_;
    uint64_t promoted_index_length_;
    char *promoted_index_;
};
class IndexFile {
    std::vector<IndexEntry> entries_;
};

class PromotedIndex {

};

class PromotedIndexBlock {

};
#endif