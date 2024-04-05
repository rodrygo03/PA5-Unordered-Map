#include "hash_functions.h"

size_t polynomial_rolling_hash::operator() (std::string const & str) const {
    /* TODO */
    size_t hash = 0;
    size_t p = 1;
    for (std::string::const_iterator i=str.begin(); i!=str.end(); i++)  {
        hash += (*i) * p;
        p = (p * 19) %  3298534883309ul;
    }
    return hash;
}

size_t fnv1a_hash::operator() (std::string const & str) const {
    /* TODO */
    size_t hash = 0xCBF29CE484222325;
    for (std::string::const_iterator i =str.begin(); i!=str.end(); i++)    {
        hash = hash ^ (*i);
        hash = hash * 0x00000100000001B3;
    }
    return hash;
}
