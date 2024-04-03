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

        // // 
        explicit basic_iterator(UnorderedMap const* map, HashNode* ptr) noexcept: _map(map), _ptr(ptr) {}

    public:
        // //
        basic_iterator():_map(nullptr), _ptr(nullptr) {}

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;
        reference operator*() const { return _ptr->val; }   // // 
        pointer operator->() const { return &(_ptr->val);}  // //
        basic_iterator& operator++() {// // 
            if (_ptr->next != nullptr)  {
                _ptr = _ptr->next;
                return *this;
            }
    
            Key k = _ptr->val.first;
            size_t t = _map->_bucket(k);
            do
            {
                t++;
            } while (_map->_buckets[t] == nullptr && t < _map->_bucket_count);
            if (t == _map->_bucket_count)   {
                _ptr = nullptr;
                return *this;
            }
            _ptr = _map->_buckets[t];
            return *this;
        }
        basic_iterator operator++(int) { // // 
            basic_iterator temp(_map, _ptr);
            ++(*this);
            return temp;
        }
        bool operator==(const basic_iterator &other) const noexcept { return _ptr == other._ptr;} // // 
        bool operator!=(const basic_iterator &other) const noexcept { return _ptr != other._ptr;} // //
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

            explicit local_iterator( HashNode * node ) noexcept: _node(node) {} // //

        public:
            local_iterator(): _node(nullptr) {} // // 

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const { return _node->val; }
            pointer operator->() const { return &(_node->val);}
            local_iterator & operator++() {     // //
                if (_node->next == nullptr) {
                    _node = nullptr;
                    return *this;
                }
                _node = _node->next;
                return *this;
            }
            local_iterator operator++(int) {    // //
                local_iterator temp(_node);
                ++(*this);
                return temp;
            }

            bool operator==(const local_iterator &other) const noexcept { return _node == other._node;} // //
            bool operator!=(const local_iterator &other) const noexcept { return _node != other._node; } // //
    };

private:

    // These helpers are used for calculating which bucket an element belongs in
    size_type _bucket(size_t code) const { // //
        return _range_hash(code, this->_bucket_count);
    }
    size_type _bucket(const Key& key) const { /* This one is used*/ 
        size_t code = _hash(key);
        return _range_hash(code, this->_bucket_count);
    }
    size_type _bucket(const value_type& val) const { // // 
        size_t code = _hash(val.first);
        return _range_hash(code, this->_bucket_count);
    }

    HashNode*& _find(size_type code, size_type bucket, const Key & key) {         
        HashNode* p = _buckets[bucket];
        while (p != nullptr)    {
            if (_equal(p->val.first, key))  {
                break;
            }
            p = p->next;
        }
        return p;
    }

    HashNode*& _find(const Key & key) { /* TODO */ 
        size_t code = _hash(key);
        size_t bucket = _bucket(key);
        return _find(code, bucket, key);
    }

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) { /* TODO */ 
        HashNode*& c = _buckets[bucket];
        _buckets[bucket] = new HashNode(std::move(value), c);
        _size++;
        if (_head == nullptr || bucket <= _bucket(_head->val))   {
            _head = _buckets[bucket];
        } 
        return _buckets[bucket];
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) { /* TODO */
        // // 
        dst._buckets = std::move(src._buckets);
        src._buckets = new HashNode*[src._bucket_count]();
    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { },
                const key_equal & equal = key_equal { }): _hash(hash) { /* TODO */ 
        _bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode*[_bucket_count]();
        _head = nullptr;
        _equal = equal;
        _hash = hash;
        _size = 0; 
    }

    ~UnorderedMap() {
        // iterator i = this->begin();
        // while (i != this->end()) {
        //     HashNode* n = i._ptr;
        //     i++;
        //     delete n;
        // }
        // delete[] _buckets;
        clear();
        delete[] _buckets;
    }

    UnorderedMap(const UnorderedMap & other) { /* TODO */
        _bucket_count = other._bucket_count;
        _size = other._size;
        _hash = other._hash;
        _equal = other._equal;
        
        _buckets = new HashNode*[_bucket_count]();
        for (size_type i=0; i<_bucket_count; i++)  {
            for (local_iterator j=other.cbegin(i); j!=other.cend(i); j++) {
                this->insert(*j);
            }
        }
    }

    UnorderedMap(UnorderedMap && other) { /* TODO */ 
        _bucket_count = other._bucket_count;
        _size = other._size;
        _hash = other._hash;
        _equal = other._equal;
        _move_content(other, *this);
        // other._bucket_count = 0;
        other._head = nullptr;
        other._size = 0;
    }

    UnorderedMap & operator=(const UnorderedMap & other) { 
        if (this == &other)  {
            return *this;
        }
        delete this;
        _bucket_count = other._bucket_count;
        _size = other._size;
        _hash = other._hash;
        _equal = other._equal;
        
        _buckets = new HashNode*[_bucket_count]();
        for (size_type i=0; i<_bucket_count; i++)  {
            for (local_iterator j=other.cbegin(i); j!=other.cend(i); j++) {
                this->insert(*j);
            }
        }
        return *this;
    }

    UnorderedMap & operator=(UnorderedMap && other) { /* TODO */
        if (this == &other)  {
            return *this;
        }
        delete this;
        _bucket_count = other._bucket_count;
        _size = other._size;
        _hash = other._hash;
        _equal = other._equal;
        _move_content(other, *this);

        other._head = nullptr;
        other._size = 0;
        return *this;
    }

    void clear() noexcept { /* TODO */
        if (_size == 0)    {
            return;
        } 
        iterator i = this->begin();
        while (i != this->end()) {
            HashNode* n = i._ptr;
            i++;
            delete n;
        }
        //delete[] _buckets;
        //_buckets = nullptr;
        _size = 0;
    }

    size_type size() const noexcept { return _size; }

    bool empty() const noexcept {return _size == 0;}

    size_type bucket_count() const noexcept { return _bucket_count; }   // //

    iterator begin() { return iterator(this, _head); }  // //
    iterator end() { return iterator(); }// // 

    const_iterator cbegin() const { return const_iterator(this, _head); } // //
    const_iterator cend() const { return const_iterator();} // //
    
    local_iterator begin(size_type n) { return local_iterator(_buckets[n]);} // //
    local_iterator end(size_type n) { return local_iterator();} // //
    local_iterator cbegin(size_type n) const {return local_iterator(_buckets[n]);}
    local_iterator cend(size_type n) const {return local_iterator();}
    

    size_type bucket_size(size_type n) { /* */ 
        size_type s=0;
        for (local_iterator i=this->begin(n); i!=this->end(n); i++)    {
            s++;
        }
        // HashNode* p = _buckets[n];
        // while (p != nullptr)    {
        //     s++;
        //     p = p->next;
        // }
        return s;
    }

    float load_factor() const { return float(_size)/float(_bucket_count);}

    size_type bucket(const Key & key) const { return _bucket(key);}


    std::pair<iterator, bool> insert(const value_type & value) { /* TODO */ 
        HashNode*& f = _find(value.first);
        if (f != nullptr)   {
            return (std::make_pair((iterator(this, f)), false));
        }
        value_type &&temp = std::make_pair(T(value.first), T(value.second)); 
        HashNode* n= _insert_into_bucket(_bucket(value), std::move(temp));
        return (std::make_pair((iterator(this, n)), true));
    } 

    std::pair<iterator, bool> insert(value_type && value) { 
        HashNode*& f = _find(value.first);
        if (f != nullptr)   {
            return (std::make_pair ((iterator(this, f)), false));
        }
        HashNode* n= _insert_into_bucket(_bucket(value), std::move(value));
        return (std::make_pair((iterator(this, n)), true));
    }

    iterator find(const Key & key) { /* TODO */
        HashNode* p = _find(key);
        return iterator(this, p);
    }

    T& operator[](const Key & key) { /* TODO */ 
        /// ????
        HashNode*& f = _find(key);
        if (f != nullptr)   {
            return f->val.second;
        }
        size_t buc = _bucket(key);
        insert(std::make_pair(key, T()));
        HashNode* newnode = _find(key);
        return newnode->val.second;
    }

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
