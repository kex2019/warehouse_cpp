#pragma once
#include <vector>
#include "../warehouse.h"
//Greedy algorithm that always takes the closest

namespace greedy {
    struct greedy {
        vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);
    };
};
