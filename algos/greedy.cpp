#include "greedy.h"
#include <iostream>
int findClosest(const vector<vector<int>> & lens, vector<bool> used, int from) {
    int minIdx = -1;
    int minDistance = 1e6;
    for(size_t i = 2; i < lens.size(); i++) {
        if(lens[0][i] < minDistance && !used[i - 2]) {
            minIdx = i - 2;
            minDistance = lens[0][i];
        }
    }
    return minIdx;
}

vector<vector<PackID>> greedy::greedy::solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse) {
    size_t nTaken = 0;
    vector<vector<PackID>> batches;
    vector<bool> used(warehouse.getPackageLocations().size(), false);
    auto &lengths = warehouse.getPathLengths();
    while(nTaken < warehouse.getPackageLocations().size()) {
        // Start with the closest package
        vector<PackID> batch;
        int minIdx = findClosest(lengths, used, 0);
        used[minIdx] = true;
        nTaken++;
        batch.push_back(minIdx);
        do {
            minIdx = findClosest(lengths, used, batch.back());
            if(minIdx == -1) {
                break;
            } 
            used[minIdx] = true;
            nTaken++;
            batch.push_back(minIdx);
        }while(batch.size() < robotCapacity);
        batches.push_back(batch);
    }
    return batches;
}
