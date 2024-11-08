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
    
    EXPECT_EQ(skipList.find_value(4), 3);
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

    EXPECT_EQ(skipList.find_value(4), 2);
    EXPECT_EQ(skipList.find_value(3), 3);

    EXPECT_EQ(skipList.update(4, 11), true);
    EXPECT_EQ(skipList.update(3, 7), true);
    skipList.display();
    EXPECT_EQ(skipList.find_value(4), 11);
    EXPECT_EQ(skipList.find_value(3), 7);
}
TEST(SkipListIteratorSuite, BasicIteration) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    skip_list_.insert(1, "one");
    skip_list_.insert(2, "two");
    skip_list_.insert(3, "three");

    std::vector<std::string> expected_values = {"one", "two", "three"};
    int index = 0;

    for (auto it = skip_list_.begin(); it != skip_list_.end(); ++it) {
        ASSERT_EQ(it->key_, index + 1);
        ASSERT_EQ(it->values_.back()->value_, expected_values[index]);
        index++;
    }
}

TEST(SkipListIteratorSuite, EmptySkipList) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    auto it = skip_list_.begin();
    ASSERT_EQ(it, skip_list_.end());
}

TEST(SkipListIteratorSuite, SingleElement) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    skip_list_.insert(1, "one");

    auto it = skip_list_.begin();
    ASSERT_NE(it, skip_list_.end());
    ASSERT_EQ(it->key_, 1);
    ASSERT_EQ(it->values_.back()->value_, "one");

    ++it;
    ASSERT_EQ(it, skip_list_.end());
}

TEST(SkipListIteratorSuite, MultipleElements) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    skip_list_.insert(1, "one");
    skip_list_.insert(3, "three");
    skip_list_.insert(2, "two");

    std::vector<std::string> expected_values = {"one", "two", "three"};
    int index = 0;

    for (auto it = skip_list_.begin(); it != skip_list_.end(); ++it) {
        ASSERT_EQ(it->key_, index + 1);
        ASSERT_EQ(it->values_.back()->value_, expected_values[index]);
        index++;
    }
}

TEST(SkipListIteratorSuite, PostIncrement) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    skip_list_.insert(1, "one");
    skip_list_.insert(2, "two");

    auto it = skip_list_.begin();
    
    auto temp_it = it++;

    ASSERT_EQ(temp_it->key_, 1);
    ASSERT_EQ(temp_it->values_.back()->value_, "one");

    ASSERT_EQ(it->key_, 2);
    ASSERT_EQ(it->values_.back()->value_, "two");
}

TEST(SkipListIteratorSuite, IteratorEquality) {
    factdb::SkipList<int, std::string> skip_list_(4, 50.0f);
    skip_list_.insert(1, "one");
    skip_list_.insert(2, "two");

    auto it1 = skip_list_.begin();
    auto it2 = skip_list_.begin();
    auto it3 = skip_list_.end();

    ASSERT_EQ(it1, it2);
    ASSERT_NE(it1, it3);
    ASSERT_NE(it2, it3);
}
