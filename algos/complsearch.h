#pragma once
#include <vector>
#include <functional>
#include "../warehouse.h"

namespace complsearch {
    struct complsearch {
        vector<vector<PackID>> solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse, function<void(long,long)> tscb);
    };
};
