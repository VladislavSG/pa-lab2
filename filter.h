#pragma once

#include <vector>
#include <functional>
#include "oneapi/tbb.h"

template<typename Iter>
Iter mid(const Iter &l, const Iter &r) {
    return l + std::distance(l, r) / 2;
}

template<typename Iter>
std::vector<typename std::iterator_traits<Iter>::value_type>
parallel2_filter(const Iter &l, const Iter &r,
                std::function<bool(const typename std::iterator_traits<Iter>::value_type &)> const &f) {
    using namespace tbb;
    using vec = std::vector<typename std::iterator_traits<Iter>::value_type>;

    std::pair<std::thread, vec> data[4];
    Iter m2 = mid(l, r);
    Iter m1 = mid(l, m2);
    Iter m3 = mid(m2, r);

    Iter it[5] = {l, m1, m2, m3, r};
    auto lambda = [f](const Iter &l, const Iter &r, vec *res) {
        for(Iter a = l; a != r; ++a)
            if (f(*a)) {
                res->emplace_back(*a);
            }
    };
    for (int i = 0; i < 4; ++i) {
        data[i].first = std::thread(
                lambda, it[i], it[i+1], &data[i].second
        );
    }
    for (auto &i : data) {
        i.first.join();
    }

    vec &res = data[0].second;
    for (int i = 1; i < 4; ++i) {
        res.insert(res.end(), data[i].second.begin(), data[i].second.end());
    }
    return res;
}

template<typename Iter>
std::vector<typename std::iterator_traits<Iter>::value_type>
parallel_filter(const Iter &l, const Iter &r,
                std::function<bool(const typename std::iterator_traits<Iter>::value_type &)> const &f) {
    using namespace tbb;
    using vec = std::vector<typename std::iterator_traits<Iter>::value_type>;
    const uint32_t n = std::distance(l, r);
    std::vector<uint32_t> flags(n, 0);
    parallel_for(blocked_range<uint32_t>(0, n),
            [&](blocked_range<uint32_t> r) {
                for (auto i = r.begin(); i != r.end(); ++i) {
                    flags[i] = f(*(l + i));
                }
            }
    );
    uint32_t res_size = parallel_scan(
            oneapi::tbb::blocked_range<uint32_t>(0, n),
            0,
            [&](const oneapi::tbb::blocked_range<uint32_t>& r, uint32_t sum, bool is_final_scan) {
                uint32_t temp = sum;
                for(uint32_t i = r.begin(); i < r.end(); ++i ) {
                    temp += flags[i];
                    if( is_final_scan )
                        flags[i] = temp;
                }
                return temp;
            },
            [](uint32_t left, uint32_t right) {
                return left + right;
            }
    );
    vec res(res_size);
    parallel_for(blocked_range<uint32_t>(0, n),
         [&](blocked_range<uint32_t> r) {
             for (auto i = r.begin(); i != r.end(); ++i) {
                 if (i == 0 && flags[0] == 1 || i != 0 && flags[i-1] != flags[i]) {
                     res[flags[i] - 1] = *(l + i);
                 }
             }
         }
    );
    return res;
}

template<typename Iter>
std::vector<typename std::iterator_traits<Iter>::value_type>
sequence_filter(const Iter &l, const Iter &r,
                std::function<bool(const typename std::iterator_traits<Iter>::value_type &)> const &f) {
    using namespace tbb;
    using vec = std::vector<typename std::iterator_traits<Iter>::value_type>;
    vec ret;
    for (auto it = l; it != r; ++it) {
        if (f(*it)) {
            ret.push_back(*it);
        }
    }
    return ret;
}
