#include "executable.h"
#include <unordered_map>

// Relies on insert, find, and global iterator
TEST(find_and_global_iterator) {
    Typegen t;

    for(size_t i = 0; i < TEST_ITER; i++) {
        using iter = typename UnorderedMap<double, double>::iterator;
        
        size_t n_pairs = t.range(1000ul);
        std::vector<std::pair<double, double>> pairs(n_pairs);
        t.fill(pairs.begin(), pairs.end());

        size_t n = t.range(100ull);

        UnorderedMap<double, double> map(n);
        std::unordered_map<double, double> shadow_map(n);

        for (auto const & pair : pairs) {
            std::pair<const double, double> to_insert(pair);
            auto [shadow_it, inserted] = shadow_map.insert(to_insert);

            std::pair<iter, bool> ret;

            {
                Memhook mh;
                ret = map.insert(to_insert);
                ASSERT_EQ(inserted, mh.n_allocs());
                ASSERT_EQ(0ULL, mh.n_frees());
            }

            // Insert Testing
            ASSERT_EQ(inserted, ret.second);
            ASSERT_EQ(shadow_map.size(), map.size());
            ASSERT_EQ(pair.first, ret.first->first);

            // Find Testing
            auto find_ret = map.find(pair.first);

            ASSERT_EQ(!inserted, find_ret == map.end());

            ASSERT_EQ(&(*find_ret), &(*ret.first));

            // Make sure [key, value]'s pointed to by ret_found are correct as well
            ASSERT_EQ(find_ret->first, pair.first);
            ASSERT_EQ(find_ret->second, pair.second);
        }

        // Test for finding a value that was not inserted, should be null and assumes t.fill for previous pairs includes negatives
        double new_key = t.range<double>(1, 100);
        while (shadow_map.find(new_key) != shadow_map.end()) {
            new_key = t.range<double>(1, 100);
        }

        // The iterator returned by map.find(new_key) should be map.end() or null, as it doesn't exist
        ASSERT_TRUE(map.find(new_key) == map.end());
    }
}
