#include <iostream>
#include <tuple>
#include <algorithm>
#include <random>
#include "warehouse.h"
#include "algos/cw.h"
#include "algos/greedy.h"

vector<long> generateSeeds(int N) {
    random_device dev;
    vector<long> seeds(N);
    for(int i = 0; i < N; i++) {
        seeds[i] = dev();
    }
    return seeds;
}

template<typename T>
vector<int> run(T t, const WarehouseInfo& info, int nRobots, int robotCapacity, const vector<long>& seeds) {
    vector<int> results(seeds.size());
    for(int i = 0; i < seeds.size(); i++) {
        Warehouse warehouse = generateRandomWarehouse(info, seeds[i]);
        auto batches = t(robotCapacity, warehouse);
        int solTime = evaluateSolutionTime(warehouse, batches, nRobots, robotCapacity);
        results[i] = solTime;
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

    auto seeds = generateSeeds(100);

    auto cws = run(cw::solve, info, 2, 5, seeds);
    auto greedys = run(greedy::solve, info, 2, 5, seeds);
    int accCWS = 0;
    int accGreedys = 0;
    for(int i = 0; i < seeds.size(); i++) {
        accCWS += cws[i];
        accGreedys += greedys[i];
    }

    cout << "ACC cws: " << accCWS << ", ACC greedy: " << accGreedys << " greedy-cws/run: " << ((double)(accGreedys-accCWS))/(double)(seeds.size()) << endl;
    /*
    Warehouse warehouse = generateRandomWarehouse(info);
    std::cout << warehouse.to_string() << endl;
    auto ret = runCW(warehouse, 2, 5);
    int savings = ret.second - ret.first;
    cout << "CW took: " << ret.first << ", naive took: " << ret.second << ", savings: " << savings << endl;*/
}