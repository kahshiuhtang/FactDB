#ifndef SKIPLIST_FACTDB_HPP
#define SKIPLIST_FACTDB_HPP

#include <iostream>
#include <cstring>
#include <stdlib.h> 

namespace factdb{
    template<typename T>
    struct Node {
        T value;
        Node** forward;

        Node(T val, int level) : value(val) {
            forward = new Node*[level + 1];
            memset(forward, 0, sizeof(Node*) * (level + 1));
        }

        ~Node() {
            delete[] forward;
        }
    };

    template<typename T>
    class SkipList {
        int max_level;
        int curr_level;
        Node<T>* header;

    public:
        SkipList(int max_level) : max_level(max_level), curr_level(0) {
            header = new Node<T>(T(), max_level);
        }

        int randomLevel() {
            int level = 0;
            while (std::rand() % 2 == 0 && level < max_level) {
                level++;
            }
            return level;
        }

        void insert(T value) {
            Node<T>* current = header;
            Node<T>* update[max_level + 1];
            std::memset(update, 0, sizeof(Node<T>*) * (max_level + 1));

            // Traverse from highest level to lowest
            for (int i = curr_level; i >= 0; i--) {
                while (current->forward[i] != nullptr && current->forward[i]->value < value) {
                    current = current->forward[i];
                }
                update[i] = current;
            }

            // Move to level 0
            current = current->forward[0];

            if (current == nullptr || current->value != value) {
                int newLevel = randomLevel();
                if (newLevel > curr_level) {
                    for (int i = curr_level + 1; i <= newLevel; i++) {
                        update[i] = header;
                    }
                    curr_level = newLevel;
                }

                Node<T>* newNode = new Node<T>(value, newLevel);
                for (int i = 0; i <= newLevel; i++) {
                    newNode->forward[i] = update[i]->forward[i];
                    update[i]->forward[i] = newNode;
                }
            }
        }

        void display() {
            for (int i = curr_level; i >= 0; i--) {
                Node<T>* node = header->forward[i];
                std::cout << "Level " << i << ": ";
                while (node != nullptr) {
                    std::cout << node->value << " ";
                    node = node->forward[i];
                }
                std::cout << "\n";
            }
        }
    };
}
#endif