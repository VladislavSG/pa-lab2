#include <iostream>
#include "bfs_seq.h"
#include "bfs_par.h"
#include <cassert>
#include <chrono>
#include <functional>
#include "utils.h"

std::size_t CUBE_SIZE = 500;
unsigned int REPEAT_SORT = 1;
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

    tbb::global_control limit4(tbb::global_control::max_allowed_parallelism, P_FACTOR);
    auto par_time = avg_bfs_time(g, bfs_par);
    std::cout  << "par = " << par_time.count() << std::endl;

    tbb::global_control limit1(tbb::global_control::max_allowed_parallelism, 1);
    auto seq_time = avg_bfs_time(g, bfs_seq);
    std::cout << "seq = " << seq_time.count() << std::endl;

    double ratio = seq_time / par_time;
    std::cout << "Speedup ratio = " << ratio << "\n";
}

void speed_test2() {
    using namespace std::chrono;
    using namespace std::chrono_literals;
    using namespace tbb;

    std::vector<int> vec(10000000);
    for (size_t i = 0; i < vec.size(); ++i) {
        vec[i] = i * i;
    }

    duration<double> t1 = time([&](){
        int sum = 0;
        for (int x : vec) {
            sum += x % 100;
        }
    });
    std::cout  << "t1 = " << t1.count() << std::endl;

    duration<double> t2 = time([&](){
        int sum = 0;
        parallel_for(blocked_range(vec.begin(), vec.end(), 10000),
            [&](blocked_range<decltype(vec)::iterator> r) {
                int lsum = 0;
                for (int const &x : r) {
                    sum += x % 100;
                }
                sum += lsum;
                //std::this_thread::sleep_for(10ms);
            }
        );
    });
    std::cout << "t2 = " << t2.count() << std::endl;

    double ratio = t1 / t2;
    std::cout << "Speedup ratio = " << ratio << "\n";
}

int main() {
    //speed_test();
    std::vector<int> test = {1, 2, -3, 4, -7, 10};
    scan(test.begin(), test.end(), 0, (bi_function<int>)std::plus());
    for (auto x : test) {
        std::cout << x << ' ';
    }
    return 0;
}
