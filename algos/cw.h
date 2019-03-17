#pragma once
#include <vector>
#include "../warehouse.h"
using namespace std;
// C&W algorithm for creating batches
namespace cw {
    vector<vector<int>> solve(int robotCapacity, const Warehouse &warehouse);
}
