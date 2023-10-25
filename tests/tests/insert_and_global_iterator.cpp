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

        std::unordered_set<std::pair<double, double>> inserted;

        for (auto const & pair : pairs) {
            inserted.insert(pair);

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

        // Traverse globally through our map, seeing if the value exists in shadow_map
        for (iter it = map.begin(); it != map.end(); it++) {
            auto found = shadow_map.find(it->first);

            tdbg << "Value with key " << it->first << " should not exist in your map.";
            ASSERT_TRUE(found != shadow_map.end());
            
            tdbg << "Key " << it->first << " has wrong value " << it->second;
            ASSERT_EQ(found->second, it->second);
        }

        // Traverse globally through shadow_map, seeing if the value exists in our map (with set of keys, because find not done)
        for (auto it = shadow_map.begin(); it != shadow_map.end(); it++) {
            auto pair = std::make_pair(it->first, it->second);
            bool is_found = inserted.find(pair) != inserted.end();

            tdbg << "Value with key " << it->first << " should exist, could not be found in your map";
            ASSERT_TRUE(is_found);

            tdbg << "Key " << it->first << " exists, but has wrong value " << it->second;
            ASSERT_EQ(it->second, pair.second);
        }

    }
}
