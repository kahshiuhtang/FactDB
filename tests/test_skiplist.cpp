#include <gtest/gtest.h>
#include <internal/skiplist.hpp>
#include <optional>

TEST(SkipListSuite, BasicInsert1) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(3, 3);
    skipList.insert(4, 3);
    skipList.display();
    EXPECT_EQ(skipList.exists(3), true);
    EXPECT_EQ(skipList.exists(4), true);
    EXPECT_EQ(skipList.find_value(3), 3);
    EXPECT_EQ(skipList.find_value(4), 3);
}

TEST(SkipListSuite, BasicInsert2) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(3, 2);
    skipList.insert(4, 3);
    skipList.display();
    EXPECT_EQ(skipList.find_value(4), 3);
    EXPECT_EQ(skipList.find_value(3), 2);
}

TEST(SkipListSuite, BasicInsert3) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(4, 2);
    skipList.insert(4, 3);
    
    EXPECT_EQ(skipList.find_value(4), 2);
}

TEST(SkipListSuite, BasicSearch1) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(3, 3);
    skipList.insert(4, 3);
    skipList.display();
    EXPECT_EQ(skipList.exists(4), true);
    EXPECT_EQ(skipList.exists(5), false);
}

TEST(SkipListSuite, BasicSearch2) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(3, 3);
    skipList.insert(4, 3);
    skipList.display();
    EXPECT_EQ(skipList.exists(4), true);
    EXPECT_EQ(skipList.exists(3), true);
}

TEST(SkipListSuite, BasicUpdate1) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(3, 2);
    skipList.insert(4, 3);

    skipList.update(4, 10);
    skipList.update(4, 11);
    EXPECT_EQ(skipList.find_value(4), 11);
    EXPECT_EQ(skipList.find_value(3), 2);
}

TEST(SkipListSuite, BasicUpdate2) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(4, 2);
    skipList.insert(4, 3);

    skipList.update(4, 10);
    skipList.update(4, 11);
    EXPECT_EQ(skipList.find_value(4), 11);
}

TEST(SkipListSuite, BasicUpdate3) {
    factdb::SkipList<int, int> skipList(4, 50.0f);
    skipList.insert(4, 2);
    skipList.insert(3, 3);

    skipList.update(4, 11);
    skipList.update(3, 7);
    EXPECT_EQ(skipList.find_value(4), 11);
    EXPECT_EQ(skipList.find_value(3), 7);
}