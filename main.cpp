#include <iostream>
#include <tuple>
#include <algorithm>
#include "warehouse.h"
#include "algos/cw.h"

// First is the solution time, second is the time it would take to take one package at a time
pair<int,int> runCW(Warehouse &warehouse, int nRobots, int robotCapacity) {
    auto batches = cw::solve(robotCapacity, warehouse);
/*    for(auto b : batches) {
        cout << "{ ";
        for(auto p : b) {
            cout << p << " ";
        }
        cout << "}" << endl;
    }*/
    vector<vector<int>> naiveBatches(warehouse.getPackageLocations().size());
    for(int i = 0; i < warehouse.getPackageLocations().size(); i++) {
        naiveBatches[i].push_back(i);
    }

    int solTime =  evaluateSolutionTime(warehouse, batches, nRobots, robotCapacity);
    int naiveTime = evaluateSolutionTime(warehouse, naiveBatches, nRobots, robotCapacity);
    return {solTime, naiveTime};
}

vector<tuple<int,int,int>> runCWs(const WarehouseInfo& info, int nRobots, int robotCapacity) {
    vector<tuple<int,int,int>> results(1000);
    for(int i = 0; i < 1000; i++) {
        Warehouse warehouse = generateRandomWarehouse(info);
        auto ret = runCW(warehouse, nRobots, robotCapacity);
        int savings = ret.second - ret.first;
        results[i] = tuple<int,int,int>(ret.first, ret.second, savings);
    }
    return results;
}

int main() {
    WarehouseInfo info;
    info.aisles = 4;
    info.aisleWidth = 2;
    info.crossAiles = 2;
    info.crossAilesWidth = 2;
    info.shelfHeight = 10;
    info.packages = 40;

    auto ret = runCWs(info, 2, 5);
    vector<int> savings(ret.size());
    for(int i = 0; i < ret.size(); i++) {
        savings[i] = get<2>(ret[i]);
    }
    sort(savings.begin(), savings.end());
    int p95 = savings[savings.size() * 0.95];
    cout << "95% savings: " << p95 << endl;
    /*
    Warehouse warehouse = generateRandomWarehouse(info);
    std::cout << warehouse.to_string() << endl;
    auto ret = runCW(warehouse, 2, 5);
    int savings = ret.second - ret.first;
    cout << "CW took: " << ret.first << ", naive took: " << ret.second << ", savings: " << savings << endl;*/
}