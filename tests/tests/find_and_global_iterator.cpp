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

        shadow_map<double, double> shadow_map(n);

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

            ASSERT_PAIRS_FOUND_IN_CORRECT_BUCKETS(shadow_map, map);

            // Find Testing
            auto ret_found = map.find(pair.first);

            ASSERT_EQ(!inserted, ret_found == map.end());

            ASSERT_EQ(&(*ret_found), &(*ret.first));
        }
    }
}
