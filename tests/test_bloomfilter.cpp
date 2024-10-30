#include <gtest/gtest.h>
#include <internal/bloomfilter.hpp>

TEST(BloomFilterSuite, InsertAndCheck) {
    factdb::BloomFilter bfilter(100, 5);
    std::string key1 = "apple";
    std::string key2 = "banana";

    bfilter.insert(key1);
    
    EXPECT_TRUE(bfilter.contains(key1)) << "Inserted key should be found in Bloom filter.";
    EXPECT_FALSE(bfilter.contains(key2)) << "Non-inserted key should not be found in Bloom filter.";
}

TEST(BloomFilterSuite, FalsePositiveRate) {
    factdb::BloomFilter bfilter(100, 5);
    std::string key1 = "cherry";
    std::string key2 = "date";
    std::string key3 = "elderberry";

    bfilter.insert(key1);
    bfilter.insert(key2);

    EXPECT_TRUE(bfilter.contains(key1)) << "Key1 should be present in Bloom filter.";
    EXPECT_TRUE(bfilter.contains(key2)) << "Key2 should be present in Bloom filter.";

    bool mightContainKey3 = bfilter.contains(key3);
    EXPECT_FALSE(mightContainKey3) << "Key3 should not be found in Bloom filter (allowing for rare false positives).";
}

TEST(BloomFilterSuite, MultipleInsertions) {
    factdb::BloomFilter bfilter(200, 5);
    std::string key1 = "fig";
    std::string key2 = "grape";
    std::string key3 = "honeydew";

    bfilter.insert(key1);
    bfilter.insert(key2);
    bfilter.insert(key3);

    EXPECT_TRUE(bfilter.contains(key1)) << "Key1 should be present in Bloom filter.";
    EXPECT_TRUE(bfilter.contains(key2)) << "Key2 should be present in Bloom filter.";
    EXPECT_TRUE(bfilter.contains(key3)) << "Key3 should be present in Bloom filter.";
}

TEST(BloomFilterSuite, EmptyFilter) {
    factdb::BloomFilter bfilter(100, 5);
    std::string key = "kiwi";

    EXPECT_FALSE(bfilter.contains(key)) << "Key should not be found in an empty Bloom filter.";
}

TEST(BloomFilterSuite, LargeKeys) {
    factdb::BloomFilter bfilter(500, 5);
    std::string largeKey(1000, 'x'); // A large key with all characters as 'x'

    bfilter.insert(largeKey);
    EXPECT_TRUE(bfilter.contains(largeKey)) << "Large key should be found in Bloom filter.";
}

TEST(BloomFilterSuite, DifferentHashCounts) {
    factdb::BloomFilter bfilter1(100, 3); // Bloom filter with fewer hash functions
    factdb::BloomFilter bfilter2(100, 10); // Bloom filter with more hash functions

    std::string key = "lemon";

    bfilter1.insert(key);
    bfilter2.insert(key);

    EXPECT_TRUE(bfilter1.contains(key)) << "Key should be found in Bloom filter 1.";
    EXPECT_TRUE(bfilter2.contains(key)) << "Key should be found in Bloom filter 2.";
}
