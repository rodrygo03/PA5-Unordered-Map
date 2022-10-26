#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"



template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    size_type _bucket_count;
    HashNode **_buckets;

    HashNode * _head;
    size_type _size;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;
        HashNode * _ptr;

        explicit basic_iterator(UnorderedMap const * map, HashNode *ptr) noexcept { /* TODO */ }

    public:
        basic_iterator() { /* TODO */ };

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;
        reference operator*() const { /* TODO */ }
        pointer operator->() const { /* TODO */ }
        basic_iterator &operator++() { /* TODO */ }
        basic_iterator operator++(int) { /* TODO */ }
        bool operator==(const basic_iterator &other) const noexcept { /* TODO */ }
        bool operator!=(const basic_iterator &other) const noexcept { /* TODO */ }
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;

            explicit local_iterator( HashNode * node ) noexcept { /* TODO */ }

        public:
            local_iterator() { /* TODO */ }

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const { /* TODO */ }
            pointer operator->() const { /* TODO */ }
            local_iterator & operator++() { /* TODO */ }
            local_iterator operator++(int) { /* TODO */ }

            bool operator==(const local_iterator &other) const noexcept { /* TODO */ }
            bool operator!=(const local_iterator &other) const noexcept { /* TODO */ }
    };

private:

    size_type _bucket(size_t code) const { /* TODO */ }
    size_type _bucket(const Key & key) const { /* TODO */ }
    size_type _bucket(const value_type & val) const { /* TODO */ }

    HashNode*& _find(size_type code, size_type bucket, const Key & key) { /* TODO */ }

    HashNode*& _find(const Key & key) { /* TODO */ }

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) { /* TODO */ }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) { /* TODO */ }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { },
                const key_equal & equal = key_equal { }) { /* TODO */ }

    ~UnorderedMap() { /* TODO */ }

    UnorderedMap(const UnorderedMap & other) { /* TODO */ }

    UnorderedMap(UnorderedMap && other) { /* TODO */ }

    UnorderedMap & operator=(const UnorderedMap & other) { /* TODO */ }

    UnorderedMap & operator=(UnorderedMap && other) { /* TODO */ }

    void clear() noexcept { /* TODO */ }

    size_type size() const noexcept { /* TODO */ }

    bool empty() const noexcept { /* TODO */ }

    size_type bucket_count() const noexcept { /* TODO */ }

    iterator begin() { /* TODO */ }
    iterator end() { /* TODO */ }

    const_iterator cbegin() const { /* TODO */ };
    const_iterator cend() const { /* TODO */ };

    local_iterator begin(size_type n) { /* TODO */ }
    local_iterator end(size_type n) { /* TODO */ }

    size_type bucket_size(size_type n) { /* TODO */ }

    float load_factor() const { /* TODO */ }

    size_type bucket(const Key & key) const { /* TODO */ }

    std::pair<iterator, bool> insert(value_type && value) { /* TODO */ }

    std::pair<iterator, bool> insert(const value_type & value) { /* TODO */ }

    iterator find(const Key & key) { /* TODO */ }

    T& operator[](const Key & key) { /* TODO */ }

    iterator erase(iterator pos) { /* TODO */ }

    size_type erase(const Key & key) { /* TODO */ }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
