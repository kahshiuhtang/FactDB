#ifndef SKIPLIST_FACTDB_HPP
#define SKIPLIST_FACTDB_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <cstring>
#include <iterator>

namespace factdb{

    template <typename ValueType>
    struct MemTableValue {
        ValueType value_;
        uint64_t timestamp_;
        bool deleted_;

        MemTableValue(ValueType v, uint64_t ts, bool del)
            : value_(v), timestamp_(ts), deleted_(del) {}
    };

    template <typename KeyType, typename ValueType>
    struct MemTableEntry {
        KeyType key_;
        bool is_deleted_ = false;
        std::vector<std::shared_ptr<MemTableValue<ValueType>>> values_;

        MemTableEntry(const KeyType& k)
            : key_(k), values_{} {}
        
        MemTableEntry(const KeyType& k, const ValueType v)
            : key_(k) {
                auto new_entry = std::make_shared<MemTableValue<ValueType>>(
                                                                v, 1633036800, false);
                values_.push_back(new_entry);
            }
    };

    template <typename KeyType, typename ValueType>
    struct SkipListNode {
        std::shared_ptr<MemTableEntry<KeyType, ValueType>> entry_;
        std::vector<std::shared_ptr<SkipListNode<KeyType, ValueType>>> forward_; // next node at each level

        SkipListNode(int level, std::shared_ptr<MemTableEntry<KeyType, ValueType>> entry)
            : entry_(entry), forward_(level + 1, nullptr) {}
    };


    template <typename KeyType, typename ValueType>
    class SkipListIterator : public std::iterator<std::forward_iterator_tag, MemTableEntry<KeyType, ValueType>> {
    public:
        SkipListIterator(std::shared_ptr<SkipListNode<KeyType, ValueType>> node)
            : current_node_(node) {}

        MemTableEntry<KeyType, ValueType>& operator*() const {
            return *current_node_->entry_;
        }

        MemTableEntry<KeyType, ValueType>* operator->() const {
            return current_node_->entry_.get();
        }

        SkipListIterator& operator++() {
            if (current_node_) {
                current_node_ = current_node_->forward_[0]; // Move to the next node at level 0
            }
            return *this;
        }

        SkipListIterator operator++(int) {
            SkipListIterator temp = *this;
            ++(*this);
            return temp;
        }
        bool operator==(std::nullptr_t) const {
            return current_node_ == nullptr;
        }

        bool operator!=(std::nullptr_t) const {
            return current_node_ != nullptr;
        }

        bool operator==(const SkipListIterator& other) const {
            return current_node_ == other.current_node_;
        }

        bool operator!=(const SkipListIterator& other) const {
            return current_node_ != other.current_node_;
        }

    private:
        std::shared_ptr<SkipListNode<KeyType, ValueType>> current_node_;
    };

    // two directions: forward and down
    template <typename KeyType, typename ValueType>
    class SkipList {
    public:
        SkipList(int max_level, float prob)
        : max_level_(max_level), next_lvl_prob_(prob) {
            auto first_entry = std::make_shared<MemTableEntry<KeyType, ValueType>>(KeyType{}, ValueType{});
            head_ = std::make_shared<SkipListNode<KeyType, ValueType>>(max_level, first_entry);
            highest_lvl_ = 0;
        }

        SkipList(int max_level)
        : max_level_(max_level), next_lvl_prob_(50.0) {
            auto first_entry = std::make_shared<MemTableEntry<KeyType, ValueType>>(KeyType{}, ValueType{});
            head_ = std::make_shared<SkipListNode<KeyType, ValueType>>(max_level, first_entry);
            highest_lvl_ = 0;
        }

        void insert(KeyType key, ValueType value) {
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_ + 1];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1, 0));
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_->key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
                to_update[i] = current;
            }

            current = current->forward_[0]; // where we should insert

            if(current == NULL || current->entry_->key_ != key){
                int r_level = random_level();
                if(r_level > highest_lvl_){
                    for(int i = highest_lvl_ + 1; i < r_level + 1; i++){
                        to_update[i] = head_; //ensure 1st element always pointing to highest as well
                    }
                    highest_lvl_ = r_level;
                }
                auto new_entry = std::make_shared<MemTableEntry<KeyType, ValueType>>(key, value);
                std::shared_ptr<SkipListNode<KeyType, ValueType>> new_node = std::make_shared<SkipListNode<KeyType, ValueType>>(
                                                                max_level_, new_entry);
                for(int i = 0; i <= r_level; i++){
                    new_node->forward_[i] = to_update[i]->forward_[i];
                    to_update[i]->forward_[i] = new_node;
                }
            }else if(current == NULL || current->entry_->key_ == key){
                auto new_entry = std::make_shared<MemTableValue<ValueType>>(value, 1633036800, false);
                current->entry_->is_deleted_ = false;
                current->entry_->values_.push_back(new_entry);
            }
        }
        bool exists(KeyType key) {
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_->key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
            }
            if(current == NULL || current->forward_[0] == NULL){
                return false;
            }
            return current->forward_[0]->entry_->key_ == key && current->entry_->is_deleted_ == false; 
        }
        std::optional<ValueType> find_value(KeyType key) {
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_->key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
            }
            current = current->forward_[0];
            std::cout << "P1: KEY" << current->entry_->key_ << std::endl;
            if(current != NULL && current->entry_->key_ == key && current->entry_->is_deleted_ == false){
                return current->entry_->values_.back()->value_;
            }
            return std::nullopt;
        }
        std::shared_ptr<SkipListNode<KeyType, ValueType>> get_head(){
            return head_;
        }
        bool update(KeyType key, ValueType value){
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            
            //start at highest level of skiplist, move current pointer forward 
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_->key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
            } 
            current = current->forward_[0];
            if(current == NULL || current->entry_->key_ != key){
                return false;
            }
            if(current && current->entry_->key_ == key && current->entry_->is_deleted_ == false){
                auto new_entry = std::make_shared<MemTableValue<ValueType>>(value, 1633036800, false);
                current->entry_->values_.push_back(new_entry);
                return true;
            }
            return false;
        }
        bool remove(KeyType key){
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_ + 1];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1));
            
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_.key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
                to_update[i] = current;
            }
            current = current->forward_[0]; // could be our desired node
            
            if(current != NULL && current->entry_.key_ == key){
                current->entry_->is_deleted_ = true;
                return true;
            }
            return false;
        }
        void display() {
            std::cout << "\n*****Skip List in [Key(Value)] format *****"<<"\n";
            for(int i=0; i <= highest_lvl_; i++)
            {
                std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_->forward_[i];
                std::cout << "Level " << i <<": ";
                while(current != NULL)
                {
                    auto entry = current->entry_->values_.back();
                    std::cout << current->entry_->key_ << "(" <<  entry->value_ << ") ";
                    current = current->forward_[i];
                }
                std::cout << "\n";
            }
        }
        factdb::SkipListIterator<KeyType, ValueType> begin() {
            return factdb::SkipListIterator<KeyType, ValueType>(head_->forward_[0]); 
        }
        factdb::SkipListIterator<KeyType, ValueType> end() {
            return factdb::SkipListIterator<KeyType, ValueType>(nullptr);
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
        bool remove_(KeyType key){
            std::shared_ptr<SkipListNode<KeyType, ValueType>> current = head_;
            std::shared_ptr<SkipListNode<KeyType, ValueType>> to_update[max_level_ + 1];
            memset(to_update, 0, sizeof(std::shared_ptr<SkipListNode<KeyType, ValueType>>) * (max_level_ + 1));
            
            for(int i = highest_lvl_; i >= 0; i--){ // top level dowm
                while(current->forward_[i] != NULL && 
                        current->forward_[i]->entry_.key_ < key){ // move as far right as possible
                            current = current->forward_[i];
                }
                to_update[i] = current;
            }
            current = current->forward_[0]; // could be our desired node
            
            if(current != NULL && current->entry_.key_ == key){
                // start from bottom, rearrange pointers
                for(int i = 0; i < highest_lvl_; i++){
                    // if next node is not target, not more rearranging needed
                    if(to_update[i]->forward_[i] != current){
                        break;
                    }
                    to_update[i]->forward_[i] = current->forward_[i];
                }

                while(highest_lvl_ > 0 and head_->forward_[highest_lvl_] == 0)
                    highest_lvl_--;
            }
            return false;
        }
    };
}
#endif