//
// Created by Pablo Brenes on 19 may 2018.
//

#include "funcionHash.h"

uint32_t hashF(const std::string key, size_t largo) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != largo) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}