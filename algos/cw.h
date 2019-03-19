#pragma once
#include <vector>
#include "../warehouse.h"
using namespace std;
// C&W algorithm for creating batches
namespace cw {
    struct cw {
        vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);
    };
}
