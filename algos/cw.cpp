#include "cw.h"
#include <iostream>
vector<vector<int>> cw::cw::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    auto &orderLocations = warehouse.getPackageLocations();
    vector<int> orderIdxs(orderLocations.size());
    vector<int> orderCost(orderLocations.size());
    for(int i = 0; i < orderIdxs.size(); i++) {
        orderIdxs[i] = i;
        orderCost[i] = warehouse.getTimeForSequence({i});
    }
    vector<pair<int,int>> orderPairs;
    vector<int> pairCost;
    vector<bool> usedOrders(orderCost.size(), false);
    int nUsed = 0;

    for(int i = 0; i < orderCost.size(); i++) {
        for(int j = 0; j < orderCost.size(); j++) {
            if(i != j) {
                orderPairs.push_back({i,j});
                pairCost.push_back(warehouse.getTimeForSequence({i,j}));
            }
        }
    }

    vector<vector<int>> batches; // All batches
    vector<int> C; // Current batch
    // Start of with finding our maximum savings
    int maxSavingsIdx = 0;
    int maxSavings = 0;
    int maxSingleCost = 0;
    for(int i = 0; i < pairCost.size(); i++) {
        int savings = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second] - pairCost[i];
        if(savings > maxSavings) {
            maxSavings = savings;
            maxSavingsIdx = i;
            maxSingleCost = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second];
        }
    }

    C = {orderPairs[maxSavingsIdx].first, orderPairs[maxSavingsIdx].second};
    int CSingleCost = maxSingleCost;
    nUsed = 2;
    usedOrders[orderPairs[maxSavingsIdx].first] = true;
    usedOrders[orderPairs[maxSavingsIdx].second] = true;    
    while(nUsed < orderCost.size()) {
        maxSavingsIdx = -1;
        maxSavings = -1;
        if(C.size() < robotCapacity) {
            //Find order with maximum savings for this
            for(int i = 0; i < orderCost.size(); i++) {
                if(!usedOrders[i]) {
                    vector<int> CC(C); // Copy C
                    CC.push_back(i);
                    int cost = warehouse.getTimeForSequence(CC); // Might need to solve tsp here actually
                    int totalSingleCost = CSingleCost + orderCost[i];
                    int savings = totalSingleCost - cost;
                    if(savings > maxSavings) {
                        maxSavings = savings;
                        maxSavingsIdx = i;
                    }
                }
            }

            if(maxSavingsIdx != -1) {
                // Add to our current batch
                CSingleCost += orderCost[maxSavingsIdx];
                usedOrders[maxSavingsIdx] = true;
                nUsed++;
                C.push_back(maxSavingsIdx);
            }
        }

        if(maxSavingsIdx == -1) {
            // This means we couldn't add to our current batch, therefore we start a new one
            batches.push_back(C);
            C = vector<int>();
            int maxSavingsIdx = -1;
            int maxSavings = -1;
            int maxSingleCost = -1;
            for(int i = 0; i < pairCost.size(); i++) {
                if(usedOrders[orderPairs[i].first] || usedOrders[orderPairs[i].second]) {
                    continue; // Don't use an order twice...
                }
                int savings = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second] - pairCost[i];
                if(savings > maxSavings) {
                    maxSavings = savings;
                    maxSavingsIdx = i;
                    maxSingleCost = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second];
                }
            }

            if(maxSavingsIdx == -1) {
                cout << "Could not find any more batches, nUsed: " << nUsed << ", total: " << orderCost.size() << endl;
                break; // We can't find any more batches
            }
            C = {orderPairs[maxSavingsIdx].first, orderPairs[maxSavingsIdx].second};
            int CSingleCost = maxSingleCost;
            usedOrders[orderPairs[maxSavingsIdx].first] = true;
            usedOrders[orderPairs[maxSavingsIdx].second] = true;
            nUsed+=2;
        }
    }

    if(C.size() > 0) {
        //cout << "C still has entries..." << endl;
        batches.push_back(C);
    }

    for(int i = 0; i < usedOrders.size(); i++) {
        if(!usedOrders[i]) {
            cout << "Order " << i << " was not put into a batch" << endl;
            batches.push_back({i});
        }
    }

    return batches;
}