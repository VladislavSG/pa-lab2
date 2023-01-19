#pragma once

#include <iterator>
#include <functional>
#include <cilk/cilk.h>
#include <numeric>

template<class Iter, class Func, class value = typename std::iterator_traits<Iter>::value_type>
value seq_scan(Iter l, Iter r, value const &init, Func const &f) {
    value sum = init;
    for (Iter it = l; it != r; ++it) {
        value sum2 = f(sum, *it);
        *it = sum;
        sum = sum2;
    }
    return sum;
}

template<class Iter, class Func, class value = typename std::iterator_traits<Iter>::value_type>
value scan(Iter l, Iter r, value const &init, Func const &f) {
    using namespace std;
    static std::ptrdiff_t const BLOCK = 400;
    ptrdiff_t dist = distance(l, r);
    if (dist < BLOCK) {
        return seq_scan(l, r, init, f);
    } else {
        size_t blocks = (dist + BLOCK - 1) / BLOCK;
        vector<value> sum(blocks);
        cilk_for (size_t pos = 0; pos < blocks; ++pos) {
            Iter block_l = l + pos * BLOCK;
            Iter block_r = pos + 1 < blocks ? block_l + BLOCK : r;
            sum[pos] = accumulate(block_l, block_r, init, f);
        }
        value res = scan(sum.begin(), sum.end(), init, f);
        cilk_for (size_t pos = 0; pos < blocks; ++pos) {
            Iter block_l = l + pos * BLOCK;
            size_t block_size = pos + 1 < blocks ? BLOCK : ((dist + BLOCK - 1) % BLOCK + 1);
            Iter block_r = block_l + block_size;
            seq_scan(block_l, block_r, sum[pos], f);
        }
        return res;
    }
}

template<class Iter, class Func, class value = typename std::iterator_traits<Iter>::value_type>
std::vector<value> parallel_filter(const Iter &l, const Iter &r,
                Func const &f) {
    size_t size = std::distance(l, r);
    std::vector<size_t> flags(size);
    cilk_for(size_t i = 0; i < flags.size(); ++i) {
        flags[i] = f(l[i]);
    }
    size_t res_size = scan(flags.begin(), flags.end(), 0ul, std::plus());
    flags.push_back(res_size);
    std::vector<value> res(res_size);
    cilk_for(size_t i = 0; i < size; ++i) {
        if (flags[i] != flags[i+1]) {
            res[flags[i]] = l[i];
        }
    }
    return res;
}
