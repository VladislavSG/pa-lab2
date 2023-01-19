#include <vector>
#include <queue>
#include "bfs_seq.h"

void bfs_seq(const graph &graph, uint32_t start) {
    std::vector<bool> visited(graph.size());
    visited[start] = true;
    std::queue<uint32_t> q;
    q.push(start);
    while (!q.empty()) {
        uint32_t v = q.front();
        q.pop();
        for (uint32_t n : graph.neighbors(v)) {
            if (!visited[n]) {
                q.push(n);
                visited[n] = true;
            }
        }
    }
}
