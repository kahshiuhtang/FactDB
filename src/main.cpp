#include <iostream>
#include "internal/skiplist.hpp"
int main(){
    std::cout << "Hello World" << std::endl; 
    factdb::SkipList<int, int> skipList(4, 50.0f);

    // Insert some values into the skip list
    std::cout << "Inserting values into SkipList...\n";
    skipList.insert(3, 3);
    skipList.insert(4, 3);

    // Display the current state of the skip list
    std::cout << "Current state of the SkipList:\n";
    skipList.display();
    if(skipList.exists(4)){
        std::cout <<  "Found 4"  << std::endl;
    }
    if(skipList.exists(5)){
        std::cout <<  "FOUND 5" << std::endl;
    }
    skipList.update(4, 7);
    // Test insertion of duplicate value
    std::cout << "Inserting duplicate value (9)...\n";
    skipList.insert(9, 3);

    // Display the skip list again
    std::cout << "State of the SkipList after attempting to insert duplicate:\n";
    skipList.display();

    // Additional tests could include removing values (if implemented), searching, etc.
    
    return 0;
}