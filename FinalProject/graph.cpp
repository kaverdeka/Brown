//
// Created by ka on 26.12.2021.
//
#include "graph.h"

namespace Graph {
    size_t getIndex(const std::string& stopName, const std::string& routeName) {
        static std::hash <std::string> hash;
        static const size_t x = 13;
        size_t r1 = hash(stopName);
        size_t r2 = hash(routeName);

        return x * r1 + r2;
    }
}
