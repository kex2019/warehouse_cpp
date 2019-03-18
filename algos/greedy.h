#pragma once
#include <vector>
#include "../warehouse.h"
//Greedy algorithm that always takes the closest

namespace greedy {
    vector<vector<int>> solve(int robotCapacity, const Warehouse &warehouse);
};
