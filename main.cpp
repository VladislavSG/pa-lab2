#include <iostream>
#include "bfs_seq.h"
#include "bfs_par.h"
#include <cassert>
#include <chrono>
#include <functional>
#include "utils.h"

std::size_t CUBE_SIZE = 300;
unsigned int REPEAT_SORT = 5;
unsigned int P_FACTOR = 4;

auto time(std::function<void()> const &f) {
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    f();
    auto end = clock::now();
    return end - start;
}

auto avg_bfs_time(const graph &g, void alg(const graph &, uint32_t)) {
    std::chrono::duration<double> sum{};
    for (unsigned int i = 0; i < REPEAT_SORT; ++i) {
        sum += time([&](){
            alg(g, 0);
        });
    }
    return sum / REPEAT_SORT;
}

void speed_test() {
    using namespace std::chrono;
    const cube_graph g(CUBE_SIZE);

    auto par_time = avg_bfs_time(g, bfs_par);
    std::cout  << "par = " << par_time.count() << std::endl;

    auto seq_time = avg_bfs_time(g, bfs_seq);
    std::cout << "seq = " << seq_time.count() << std::endl;

    double ratio = seq_time / par_time;
    std::cout << "Speedup ratio = " << ratio << "\n";
}


int main() {
    speed_test();
    return 0;
}
