#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <limits>

class graph {
    using neighbors_t = std::vector<uint32_t>;
public:
    [[nodiscard]] virtual std::size_t size() const = 0;

    [[nodiscard]] virtual std::vector<uint32_t> neighbors(uint32_t v) const = 0;
};

class cube_graph : public graph {
    uint32_t const side_length;
    uint32_t const side2_length;
    uint32_t const side3_length;

    void step(std::vector<uint32_t> &res, uint32_t vertex, uint32_t delta) const;

public:
    explicit cube_graph(uint32_t side);

    [[nodiscard]] std::size_t size() const override;

    [[nodiscard]] std::vector<uint32_t> neighbors(uint32_t v) const override;
};
