#pragma once
#include <vector>
#include "../warehouse.h"
//Greedy algorithm that always takes the closest

namespace greedy {
    struct greedy {
        vector<vector<PackID>> solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse);
    };
};
