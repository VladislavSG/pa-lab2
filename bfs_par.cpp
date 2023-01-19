#pragma GCC optimize("Ofast")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,popcnt,abm,mmx")
#pragma GCC optimize("fast-math")

#include "bfs_par.h"
#include <utility>
#include <iostream>
#include "filter.h"
//#include <cilk/cilk.h>

void bfs_par(const graph &g, uint32_t start) {
    using namespace tbb;
    using namespace std;
    vector<uint32_t> frontier{start};
    vector<atomic_bool> visited(g.size());
    visited[start] = true;
    while (!frontier.empty()) {
        std::vector<uint32_t> deg;
        deg.reserve(frontier.size());
        for (uint32_t v : frontier) {
            deg.push_back(g.neighbors(v).size());
        }
        std::size_t front_size = parallel_scan(
            oneapi::tbb::blocked_range<uint32_t>(0,deg.size()),
            0,
            [&deg](const oneapi::tbb::blocked_range<uint32_t>& r, uint32_t sum, bool is_final_scan) {
                uint32_t temp = sum;
                for(uint32_t i = r.begin(); i < r.end(); ++i ) {
                    temp += deg[i];
                    if( is_final_scan )
                        deg[i] = temp;
                }
                return temp;
            },
            [](uint32_t left, uint32_t right) {
                return left + right;
            }
        );
        std::vector<uint32_t> new_frontier(front_size, -1);

        for (int vi = 0; vi < frontier.size(); ++vi) {
            const auto &neighbors = g.neighbors(frontier[vi]);

            for (int ui = 0; ui < neighbors.size(); ++ui) {
                uint32_t u = neighbors[ui];
                bool expected = false;
                if (visited[u].compare_exchange_strong(expected, true)) {
                    new_frontier[deg[vi] - ui - 1] = u;
                }
            }
        }

//        parallel_for(static_cast<size_t>(0), frontier.size(),
//            [&](size_t vi){
//                const auto &neighbors = g.neighbors(frontier[vi]);
//
//                parallel_for(static_cast<size_t>(0), neighbors.size(),
//                    [&](size_t ui){
//                        uint32_t u = neighbors[ui];
//                        bool expected = false;
//                        if (visited[u].compare_exchange_strong(expected, true)) {
//                            new_frontier[deg[vi] - ui - 1] = u;
//                        }
//                    }
//                );
//            }
//        );

        frontier = parallel_filter(new_frontier.begin(), new_frontier.end(),
                                   [](const uint32_t &x){ return x != -1; });
    }
}