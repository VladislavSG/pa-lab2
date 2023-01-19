#pragma GCC optimize("Ofast")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,popcnt,abm,mmx")
#pragma GCC optimize("fast-math")

#include "bfs_par.h"
#include <utility>
#include <atomic>
#include <cilk/cilk.h>
#include "utils.h"


void deg_init(const graph &g, const std::vector<uint32_t> &frontier, std::vector<uint32_t> &deg) {
    cilk_for (std::size_t i = 0; i < frontier.size(); ++i) {
        deg[i] = g.neighbors_size(frontier[i]);
    }
}

void iterate_neighbours(std::vector<std::atomic_bool> &visited, const std::vector<uint32_t> &deg,
                                          std::vector<uint32_t> &new_frontier, int vi,
                                          const std::vector<uint32_t> &neighbors) {
    cilk_for (int ui = 0; ui < neighbors.size(); ++ui) {
        uint32_t u = neighbors[ui];
        bool expected = false;
        if (visited[u].compare_exchange_strong(expected, true)) {
            new_frontier[deg[vi] + ui] = u;
        }
    };
}

void bfs_par(const graph &g, uint32_t start) {
    using namespace std;
    vector<uint32_t> frontier{start};
    vector<atomic_bool> visited(g.size());
    visited[start] = true;
    while (!frontier.empty()) {
        std::vector<uint32_t> deg(frontier.size());
        deg_init(g, frontier, deg);
        std::size_t front_size = scan(deg.begin(), deg.end(), 0ul, std::plus());
        std::vector<uint32_t> new_frontier(front_size, -1);

        cilk_for (int vi = 0; vi < frontier.size(); ++vi) {
            const auto &neighbors = g.neighbors(frontier[vi]);

            iterate_neighbours(visited, deg, new_frontier, vi, neighbors);
        }

        frontier = parallel_filter(new_frontier.begin(), new_frontier.end(),
                                   [](const uint32_t &x){ return x != -1; });
    }
}