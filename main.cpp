#include <iostream>
#include "bfs_seq.h"
#include "bfs_par.h"
#include <cassert>
#include <chrono>
#include <functional>

std::size_t CUBE_SIZE = 300;
unsigned int REPEAT_SORT = 5;

auto time(std::function<void()> const &f) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    f();
    auto end = clock::now();
    return end - start;
}

auto avg_bfs_time(const graph &g, std::vector<std::size_t> alg(const graph &, uint32_t), std::vector<std::size_t> &dist) {
    std::chrono::duration<double> sum{};
    for (unsigned int i = 0; i < REPEAT_SORT; ++i) {
        std::vector<std::size_t> dist2;
        sum += time([&](){
            dist2 = alg(g, 0);
        });
        if (i == 0) {
            dist = dist2;
        } else {
            assert(dist == dist2);
        }
    }
    return sum / REPEAT_SORT;
}

void speed_test() {
    using namespace std::chrono;
    const cube_graph g(CUBE_SIZE);

    std::vector<std::size_t> dist_par;
    auto par_time = avg_bfs_time(g, bfs_par, dist_par);
    std::cout  << "par = " << par_time.count() << std::endl;

    std::vector<std::size_t> dist_seq;
    auto seq_time = avg_bfs_time(g, bfs_seq, dist_seq);
    std::cout << "seq = " << seq_time.count() << std::endl;

    assert(dist_par == dist_seq);

    double ratio = seq_time / par_time;
    std::cout << "Speedup ratio = " << ratio << "\n";
}


int main() {
    speed_test();
    return 0;
}
