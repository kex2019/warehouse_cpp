#include "cw.h"
#include <iostream>
#include <algorithm>
vector<vector<PackID>> cw::cw::solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse) {
    auto &orderLocations = warehouse.getPackageLocations();
    vector<int> orderIdxs(orderLocations.size());
    vector<int> orderCost(orderLocations.size());
    for(size_t i = 0; i < orderIdxs.size(); i++) {
        orderIdxs[i] = i;
        orderCost[i] = warehouse.getTimeForSequence(vector<PackID>{static_cast<PackID>(i)});
    }
    vector<pair<PackID,PackID>> orderPairs;
    vector<pair<int,int>> pairCost;
    
    for(size_t i = 0; i < orderCost.size(); i++) {
        for(size_t j = 0; j < orderCost.size(); j++) {
            if(i != j) {
                PackID idx = static_cast<PackID>(i);
                PackID jdx = static_cast<PackID>(j);
                orderPairs.push_back({idx,jdx});
                pairCost.push_back({warehouse.getTimeForSequence(vector<PackID>{idx,jdx}), orderPairs.size() - 1});
            }
        }
    }

    sort(pairCost.begin(), pairCost.end());
    reverse(pairCost.begin(), pairCost.end());
    vector<int> orderBatchIdx(orderLocations.size(), -1);
    vector<vector<PackID>> batches;
    for(size_t i = 0; i < orderPairs.size(); i++) {
        auto cur = pairCost[i];
        auto orders = orderPairs[cur.second];
        // Both not in batch
        if(orderBatchIdx[orders.first] == -1 && orderBatchIdx[orders.second] == -1) {
            // Add to their own batch
            int idx = batches.size();
            batches.push_back({orders.first, orders.second});
            orderBatchIdx[orders.first] = idx;
            orderBatchIdx[orders.second] = idx;
        } else if(orderBatchIdx[orders.first] == -1 && batches[orderBatchIdx[orders.second]].size() < robotCapacity) {
            // Add orders.first to orders.second's batch
            orderBatchIdx[orders.first] = orderBatchIdx[orders.second];
            batches[orderBatchIdx[orders.first]].push_back(orders.first);
        } else if(orderBatchIdx[orders.second] == -1 && batches[orderBatchIdx[orders.first]].size() < robotCapacity) {
            // Add orders.second to orders.first's batch
            orderBatchIdx[orders.second] = orderBatchIdx[orders.first];
            batches[orderBatchIdx[orders.second]].push_back(orders.second);
        } else {
            // Can't add to a batch, just keep on going
        }
    }

    for(size_t i = 0; i < orderBatchIdx.size(); i++) {
        if(orderBatchIdx[i] == -1) {
            cout << "Order " << i << " could not find a batch" << endl;
        }
    }

    return batches;
}