#include "greedy.h"
#include <iostream>
int findClosest(const vector<vector<int>> & lens, vector<bool> used, int from) {
    int minIdx = -1;
    int minDistance = 1e6;
    for(int i = 2; i < lens.size(); i++) {
        if(lens[0][i] < minDistance && !used[i - 2]) {
            minIdx = i - 2;
            minDistance = lens[0][i];
        }
    }
    return minIdx;
}

vector<vector<int>> greedy::greedy::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int nTaken = 0;
    vector<vector<int>> batches;
    vector<bool> used(warehouse.getPackageLocations().size(), false);
    auto &lengths = warehouse.getPathLengths();
    while(nTaken < warehouse.getPackageLocations().size()) {
        // Start with the closest package
        vector<int> batch;
        int minIdx = findClosest(lengths, used, 0);
        used[minIdx] = true;
        nTaken++;
        batch.push_back(minIdx);
        do {
            minIdx = findClosest(lengths, used, batch.back());
            // TODO: DEBUG You fail...
            if(minIdx == -1) {
                cerr << "Could not find minimum" << endl;
                throw runtime_error("Could not find minimum");
            } 
            used[minIdx] = true;
            nTaken++;
            batch.push_back(minIdx);
        }while(batch.size() < robotCapacity);
        batches.push_back(batch);
    }
    return batches;
}
