#include "executable.h"
#include <unordered_map>
#include <unordered_set>

TEST(insert_and_global_iterator) {
    Typegen t;
    for(size_t i = 0; i < TEST_ITER; i++) {
        using iter = typename UnorderedMap<double, double>::iterator;

        size_t n_pairs = t.range(400ul);
        size_t n = t.range(1000ull);

        std::vector<std::pair<double, double>> pairs(n_pairs);
        t.fill(pairs.begin(), pairs.end());

        std::unordered_map<double, double> shadow_map;
        UnorderedMap<double, double> map(n);

        std::unordered_set<std::pair<double, double>> shadow_inserted_pairs;

        for (auto const & pair : pairs) {
            std::pair<const double, double> to_insert(pair);
            auto [it, inserted] = shadow_map.insert(to_insert);
            
            std::pair<iter, bool> ret;
            
            {
                Memhook mh;
                ret = map.insert(to_insert);
                ASSERT_EQ(inserted, mh.n_allocs());
            }

            ASSERT_EQ(inserted, ret.second);
            ASSERT_EQ(shadow_map.size(), map.size());
            ASSERT_EQ(pair.first, ret.first->first);
        }

        // Traverse globally through shadow map, as we know each pair here was properly inserted
        for (auto it = shadow_map.begin(); it != shadow_map.end(); it++) {
            auto pair = std::make_pair(it->first, it->second);
            shadow_inserted_pairs.insert(pair);
        }

        // Make sure everything in our map is in the other map.
        for (iter it = map.begin(); it != map.end(); it++) {
            bool found = shadow_inserted_pairs.find(std::make_pair(it->first, it->second)) != shadow_inserted_pairs.end();

            tdbg << "Value with key " << it->first << " and value " << it->second << " should not exist in your map.";
            ASSERT_TRUE(found);
        }
    }
}
