#pragma once
#include <iterator>
#include <functional>
//#include <cilk/cilk.h>
#include <numeric>

template<typename T>
using bi_function = std::function<T(T const &, T const &)>;

template<class Iter, class value = typename std::iterator_traits<Iter>::value_type>
value seq_scan(Iter l, Iter r, value const &init, bi_function<value> const &f) {
    value sum = init;
    for (Iter it = l; it != r; ++it) {
        value sum2 = f(sum, *l);
        *l = sum;
        sum = sum2;
    }
    return sum;
}

template<class Iter, class value = typename std::iterator_traits<Iter>::value_type>
value scan(Iter l, Iter r, value const &init, bi_function<value> const &f) {
    using namespace std;
    static std::ptrdiff_t const BLOCK = 1000;
    ptrdiff_t dist = distance(l, r);
    if (dist < BLOCK) {
        return seq_scan(l, r, init, f);
    } else {
        size_t blocks = (dist + BLOCK - 1) / BLOCK;
        vector<value> sum(blocks);
        for (size_t pos = 0; pos < blocks; ++pos) {
            Iter block_l = l + pos * BLOCK;
            Iter block_r = pos + 1 < blocks ? block_l + BLOCK : r;
            sum[pos] = accumulate(block_l, block_r, init, f);
        }
        value res = scan(sum.begin(), sum.end(), init, f);
        for (size_t pos = 0; pos < blocks; ++pos) {
            Iter block_l = l + pos * BLOCK;
            size_t block_size = pos + 1 < blocks ? BLOCK : ((dist + BLOCK - 1) % BLOCK + 1);
            for (size_t i = 0; i < block_size; ++i) {
                Iter it = block_l + i;
                *it = f(*it, sum[pos]);
            }
        }
        return res;
    }
}
