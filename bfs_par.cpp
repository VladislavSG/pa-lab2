#include "bfs_par.h"
#include <utility>
#include <atomic>
#include <cilk/cilk.h>
#include "utils.h"

std::vector<std::size_t> bfs_par(const graph &g, uint32_t start) {
    using namespace std;
    vector<uint32_t> frontier{start};
    vector<atomic_bool> visited(g.size());
    visited[start] = true;

    vector<size_t> dist(g.size());

    while (!frontier.empty()) {
        std::vector<uint32_t> deg(frontier.size());
        cilk_for (std::size_t i = 0; i < frontier.size(); ++i) {
            deg[i] = g.neighbors_size(frontier[i]);
        }
        std::size_t front_size = scan(deg.begin(), deg.end(), 0ul, std::plus());
        std::vector<uint32_t> new_frontier(front_size, -1);

        cilk_for (int vi = 0; vi < frontier.size(); ++vi) {
            const auto &neighbors = g.neighbors(frontier[vi]);
            cilk_for (int ui = 0; ui < neighbors.size(); ++ui) {
                uint32_t u = neighbors[ui];
                bool expected = false;
                if (visited[u].compare_exchange_strong(expected, true)) {
                    new_frontier[deg[vi] + ui] = u;
                    dist[u] = dist[frontier[vi]] + 1;
                }
            };
        }

        frontier = parallel_filter(new_frontier.begin(), new_frontier.end(),
                                   [](const uint32_t &x){ return x != -1; });
    }

    return dist;
}