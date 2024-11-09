#ifndef DATAFILE_FACTDB_HPP
#define DATAFILE_FACTDB_HPP

#include <cstdint>
#include <optional>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>

namespace factdb {
const int DEFAULT_ARRAY_SIZE = 32;

class DeletionTime {
public:
    uint32_t local_deletion_time;
    uint64_t marked_for_delete_at;

    DeletionTime() : local_deletion_time(0), marked_for_delete_at(0) {}
private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & local_deletion_time;
        ar & marked_for_delete_at;
    }
};

class PartitionHeader {
public:
    uint16_t key_length_;
    std::vector<char> key_;
    std::vector<DeletionTime> deletion_info_;

    PartitionHeader() : key_length_(0), deletion_info_(DEFAULT_ARRAY_SIZE) {}
};

class Unfiltered {};

class Cell {};


class DeltaDeletionTime {
public:
    uint64_t delta_marked_for_delete_at;
    uint64_t delta_local_deletion_time;

    DeltaDeletionTime() : delta_marked_for_delete_at(0), delta_local_deletion_time(0) {}
};

class LivenessInfo {
public:
    uint64_t delta_timestamp_;
    std::optional<uint64_t> delta_ttl_;
    std::optional<uint64_t> delta_local_deletion_time_;

    LivenessInfo() : delta_timestamp_(0) {}
};

enum class RowFlags {
    END_OF_PARTITION = 0x01,
    IS_MARKER = 0x02,
    HAS_TIMESTAMP = 0x04,
    HAS_TTL = 0x08,
    HAS_DELETION = 0x10,
    HAS_ALL_COLUMNS = 0x20,
    HAS_COMPLEX_DELETION = 0x40,
    EXTENSION_FLAG = 0x80
};

enum class ExtendedRowFlags {
    IS_STATIC = 0x01,
    HAS_SHADOWABLE_DELETION_CASSANDRA = 0x02,
    HAS_SHADOWABLE_DELETION_SCYLLA = 0x80,
};
class CellPath {
public:
    uint32_t length_;
    std::vector<char> value_;

    CellPath(uint32_t length) : length_(length), value_(length) {}
};
class CellValue {
public:
    uint32_t val_length_;
    uint32_t key_length_;
    std::vector<char> key_;
    std::vector<char> value_;

    CellValue() {}
    CellValue(uint32_t val_length, uint32_t key_length) : val_length_(val_length), value_(key_length) {}
};
class SimpleCell : public Cell {
public:
    char flags;
    std::optional<uint64_t> delta_timestamp_;
    std::optional<uint64_t> delta_local_deletion_time_;
    std::optional<uint64_t> delta_ttl_;
    std::optional<CellPath> path_; // only in cells nested into complex_cells
    std::optional<CellValue> value_;

    SimpleCell() : flags(0) {}
    SimpleCell(const CellValue& cell_value) : flags(0), value_(cell_value) {}
};
class ClusteringBlock {
public:
    uint64_t clustering_block_header_;
    std::vector<SimpleCell> clustering_cells_;
};

enum class CellFlags {
    IS_DELETED_MASK = 0x01,
    IS_EXPIRING_MASK = 0x02,
    HAS_EMPTY_VALUE_MASK = 0x04,
    USE_ROW_TIMESTAMP_MASK = 0x08,
    USE_ROW_TTL_MASK = 0x10,
};

class ComplexCell : public Cell {
public:
    std::optional<DeltaDeletionTime> complex_deletion_time;
    uint32_t items_count;
    std::vector<SimpleCell> value_;

    ComplexCell() : items_count(0) {}
};

class RangeTombstoneMarker : public Unfiltered {
public:
    RowFlags flags_ = RowFlags::IS_MARKER;
    char kind_ordinal_;
    uint16_t bound_values_count_;
    ClusteringBlock* clustering_blocks_;
    uint32_t marker_body_size_;
    uint32_t prev_unfiltered_size_;

    RangeTombstoneMarker() : kind_ordinal_(0), bound_values_count_(0), clustering_blocks_(nullptr), marker_body_size_(0), prev_unfiltered_size_(0) {}
};

enum class BoundKind : uint8_t {
    EXCL_END_BOUND = 0,
    INCL_START_BOUND = 1,
    EXCL_END_INCL_START_BOUNDARY = 2,
    STATIC_CLUSTERING = 3,
    CLUSTERING = 4,
    INCL_END_EXCL_START_BOUNDARY = 5,
    INCL_END_BOUND = 6,
    EXCL_START_BOUND = 7
};
class RangeTombstoneBoundMarker : public RangeTombstoneMarker {
public:
    DeltaDeletionTime deletion_time_;
};
class RangeTombstoneBoundaryMarker : public RangeTombstoneMarker {
public:
    DeltaDeletionTime end_deletion_time_;
    DeltaDeletionTime start_deletion_time_;
};
class Row : public Unfiltered {
public:
    char flags_;
    std::optional<char> extended_flags_;
    std::vector<std::shared_ptr<factdb::ClusteringBlock>>  clustering_blocks_;
    uint64_t row_body_size_;
    uint64_t prev_unfiltered_size_; // for backward traversing
    std::optional<LivenessInfo> liveness_info_;
    std::optional<DeltaDeletionTime> deletion_time_;
    std::vector<std::optional<uint64_t>> missing_columns_;
    std::vector<Cell> cells_;

    Row() : flags_(0), row_body_size_(0), prev_unfiltered_size_(0) {}
};

class Partition {
public:
    PartitionHeader header_;
    std::optional<Row> static_row_;
    std::vector<std::shared_ptr<Unfiltered>> unfiltereds_;

    Partition() : header_(), static_row_(std::nullopt) {}
};

} // namespace factdb

#endif // DATAFILE_FACTDB_HPP
