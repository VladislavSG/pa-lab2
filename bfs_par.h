#pragma once

#include "graph.h"
#include "oneapi/tbb.h"

void bfs_par(const graph &g, uint32_t start);
