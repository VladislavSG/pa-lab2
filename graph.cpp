#pragma GCC optimize("Ofast")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,popcnt,abm,mmx")
#pragma GCC optimize("fast-math")

#include "graph.h"

std::size_t cube_graph::step_size(uint32_t vertex, uint32_t delta) const {
    return std::size_t(vertex >= delta) + std::size_t(vertex + delta < side3_length);
}

void cube_graph::step(std::vector<uint32_t> &res, uint32_t vertex, uint32_t delta) const {
    if (vertex >= delta) {
        res.emplace_back(vertex - delta);
    }
    uint32_t sum = vertex + delta;
    if (sum < side3_length) {
        res.emplace_back(sum);
    }
}

cube_graph::cube_graph(uint32_t side)
    : side_length(side)
    , side2_length(side * side)
    , side3_length(side2_length * side) {}

std::size_t cube_graph::size() const {
    return side2_length * side_length;
}

std::vector<uint32_t> cube_graph::neighbors(uint32_t v) const {
    std::vector<uint32_t> res;
    res.reserve(6);
    step(res, v, 1);
    step(res, v, side_length);
    step(res, v, side2_length);
    return res;
}

std::size_t cube_graph::neighbors_size(uint32_t v) const {
    return step_size(v, 1)
         + step_size(v, side_length)
         + step_size(v, side2_length);
}
