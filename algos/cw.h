#pragma once
#include <vector>
#include "../warehouse.h"
using namespace std;
// C&W algorithm for creating batches
namespace cw {
    struct cw {
        vector<vector<PackID>> solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse);
    };
}
