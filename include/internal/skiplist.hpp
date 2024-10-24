#ifndef SKIPLIST_FACTDB_HPP
#define SKIPLIST_FACTDB_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include <random>
#include <string>

namespace factdb{

    template <typename KeyType, typename ValueType>
    struct MemTableEntry {
        KeyType key;
        std::optional<ValueType> value;
        uint64_t timestamp;
        bool deleted;

        MemTableEntry(const KeyType& k, const std::optional<ValueType>& v, uint64_t ts, bool del)
            : key(k), value(v), timestamp(ts), deleted(del) {}
    };

    template <typename KeyType, typename ValueType>
    struct SkipListNode {
        MemTableEntry<KeyType, ValueType> entry;
        std::vector<std::shared_ptr<SkipListNode<KeyType, ValueType>>> forward; // Forward pointers for each level

        SkipListNode(int level, const MemTableEntry<KeyType, ValueType>& entry)
            : entry(entry), forward(level, nullptr) {}
    };

    template <typename KeyType, typename ValueType>
    class SkipList {
    public:
        SkipList(int max_level, float prob)
        : max_level(max_level), probability(prob) {
            head = std::make_shared<SkipListNode<KeyType, ValueType>>(max_level, MemTableEntry<KeyType, ValueType>(KeyType{}, std::nullopt, 0, false));
            curr_level = 0;
        }
        void insert(KeyType key, ValueType value) {
        }
        bool search(KeyType value){
            return false;
        }
        bool update(KeyType key, ValueType value){
            return false;
        }
        bool remove(KeyType value){
            return false;
        }
        void display() {
            
        }
    private:
        std::shared_ptr<SkipListNode<KeyType, ValueType>> head;   // Head node of the skiplist
        int max_level;                                             // Maximum levels in the skiplist
        int curr_level;                                        // Current top level
        float probability; 
        int random_level() {
            int level = 0;
            while (std::rand() % 2 == 0 && level < max_level) {
                level++;
            }
            return level;
        }
    };
}
#endif