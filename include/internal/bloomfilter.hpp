#ifndef BFILTER_FACTDB_HPP 
#define BFILTER_FACTDB_HPP

#include <vector>
#include <string>
#include <boost/functional/hash.hpp>

namespace factdb {

class BloomFilter {
public:
    BloomFilter(size_t size, size_t numHashes) 
        : bits(size), numHashes(numHashes) {}

    void insert(const std::string& key) {
        for (size_t i = 0; i < numHashes; ++i) {
            bits[hash(key, i) % bits.size()] = true;
        }
    }

    bool contains(const std::string& key) const {
        for (size_t i = 0; i < numHashes; ++i) {
            if (!bits[hash(key, i) % bits.size()]) {
                return false;
            }
        }
        return true;
    }

private:
    std::vector<bool> bits;
    size_t numHashes;

    size_t hash(const std::string& key, size_t seed) const {
        boost::hash<std::string> stringHash;
        return stringHash(key) ^ (seed * 0x9e3779b9);
    }
};
}
#endif