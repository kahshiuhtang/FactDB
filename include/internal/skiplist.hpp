#ifndef SKIPLIST_FACTDB_HPP
#define SKIPLIST_FACTDB_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <cstring>

namespace factdb{

    template <typename KeyType, typename ValueType>
    struct MemTableEntry {
        KeyType key_;
        std::optional<ValueType> value_;
        uint64_t timestamp_;
        bool deleted_;

        MemTableEntry(const KeyType& k, const std::optional<ValueType>& v, uint64_t ts, bool del)
            : key_(k), value_(v), timestamp_(ts), deleted_(del) {}
    };

    template <typename KeyType, typename ValueType>
    struct SkipListNode {
        MemTableEntry<KeyType, ValueType> entry_;
        std::vector<std::shared_ptr<SkipListNode<KeyType, ValueType>>> forward_; // next node at each level

        SkipListNode(int level, const MemTableEntry<KeyType, ValueType>& entry)
            : entry_(entry), forward_(level, nullptr) {}
    };
    // two directions: forward and down
    template <typename KeyType, typename ValueType>
    class SkipList {
    public:
        SkipList(int max_level, float prob)
        : max_level_(max_level), next_lvl_prob_(prob) {
            head_ = std::make_shared<SkipListNode<KeyType, ValueType>>(max_level, MemTableEntry<KeyType, ValueType>(
                KeyType{}, std::nullopt, 0, false));
            highest_lvl_ = 0;
        }

        SkipList(int max_level)
        : max_level_(max_level), next_lvl_prob_(50.0) {
            head_ = std::make_shared<SkipListNode<KeyType, ValueType>>(max_level, MemTableEntry<KeyType, ValueType>(
                KeyType{}, std::nullopt, 0, false));
            highest_lvl_ = 0;
        }

        void insert(KeyType key, ValueType value) {
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1));
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_.key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
                to_update[i] = current;
            }

            current = current->forward_[0]; // where we should insert

            if(current == NULL || current->entry_.key_ != key){
                int r_level = random_level();
                if(r_level > highest_lvl_){
                    for(int i = highest_lvl_ + 1; i < r_level + 1; i++){
                        to_update[i] = head_; //ensure 1st element always pointing to highest as well
                    }
                    highest_lvl_ = r_level;
                }

                std::shared_ptr<SkipListNode<KeyType, ValueType>> new_node = std::make_shared<SkipListNode<KeyType, ValueType>>(
                                                                max_level_, MemTableEntry<KeyType, ValueType>
                                                                (key, value, 0, false));
                for(int i = 0; i < r_level; i++){
                    new_node->forward_[i] = to_update[i]->forward_[i];
                    to_update[i]->forward_[i] = new_node;
                }
            }
        }

        bool search(KeyType key) {
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1));
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_.key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
            }
            if(current == NULL || current->forward_[0] == NULL){
                return false;
            }
            return current->forward_[0]->entry_.key_ == key; 
        }
        bool update(KeyType key, ValueType value){
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1));
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_.key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
            }
            if(current == NULL || current->forward_[0] == NULL){
                return false;
            }
            current->forward_[0]->entry_.value_ = value;
            return true;
        }
        bool remove(KeyType value){

            return false;
        }
        void display() {
            
        }
    private:
        std::shared_ptr<SkipListNode<KeyType, ValueType>> head_;     // head node of the skiplist
        int max_level_;                                             // maximum levels in the skiplist
        int highest_lvl_;                                           // current top level
        float next_lvl_prob_;                                       // maxiumum 

        int random_level() {
            int level = 0;
            while (std::rand() % 2 == 0 && level < max_level_) {
                level++;
            }
            return level;
        }
    };
}
#endif