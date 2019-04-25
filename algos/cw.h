#pragma once
#include <vector>
#include "../warehouse.h"
#include <functional>
using namespace std;
// C&W algorithm for creating batches
namespace cw {
    struct cw {
        vector<vector<PackID>> solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse, function<void(long,long)> tscb);
    };
}
