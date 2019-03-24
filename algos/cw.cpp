#include "cw.h"
#include <iostream>
#include <algorithm>
vector<vector<int>> cw::cw::solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse) {
    auto &orderLocations = warehouse.getPackageLocations();
    vector<int> orderIdxs(orderLocations.size());
    vector<int> orderCost(orderLocations.size());
    for(size_t i = 0; i < orderIdxs.size(); i++) {
        orderIdxs[i] = i;
        orderCost[i] = warehouse.getTimeForSequence({static_cast<int>(i)});
    }
    vector<pair<int,int>> orderPairs;
    vector<pair<int,int>> pairCost;
    
    for(size_t i = 0; i < orderCost.size(); i++) {
        for(size_t j = 0; j < orderCost.size(); j++) {
            if(i != j) {
                int idx = static_cast<int>(i);
                int jdx = static_cast<int>(j);
                orderPairs.push_back({idx,jdx});
                pairCost.push_back({warehouse.getTimeForSequence({idx,jdx}), orderPairs.size() - 1});
            }
        }
    }

    sort(pairCost.begin(), pairCost.end());
    reverse(pairCost.begin(), pairCost.end());
    vector<int> orderBatchIdx(orderLocations.size(), -1);
    vector<vector<int>> batches;
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

/*
    vector<bool> usedOrders(orderCost.size(), false);

    vector<vector<int>> batches; // All batches
    vector<int> C; // Current batch
    // Start of with finding our maximum savings
    int maxSavingsIdx = 0;
    int maxSavings = 0;
    int maxSingleCost = 0;
    for(size_t i = 0; i < pairCost.size(); i++) {
        int savings = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second] - pairCost[i];
        if(savings > maxSavings) {
            maxSavings = savings;
            maxSavingsIdx = i;
            maxSingleCost = orderCost[orderPairs[i].first] + orderCost[orderPairs[i].second];
        }
    }

    C = {orderPairs[maxSavingsIdx].first, orderPairs[maxSavingsIdx].second};
    int CSingleCost = maxSingleCost;
    size_t nUsed = 2;
    usedOrders[orderPairs[maxSavingsIdx].first] = true;
    usedOrders[orderPairs[maxSavingsIdx].second] = true;    
    while(nUsed < orderCost.size()) {
        maxSavingsIdx = -1;
        maxSavings = -1;
        if(C.size() < robotCapacity) {
            //Find order with maximum savings for this
            for(size_t i = 0; i < orderCost.size(); i++) {
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
            for(size_t i = 0; i < pairCost.size(); i++) {
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
            CSingleCost = maxSingleCost;
            usedOrders[orderPairs[maxSavingsIdx].first] = true;
            usedOrders[orderPairs[maxSavingsIdx].second] = true;
            nUsed+=2;
        }
    }

    if(C.size() > 0) {
        //cout << "C still has entries..." << endl;
        batches.push_back(C);
    }

    for(size_t i = 0; i < usedOrders.size(); i++) {
        if(!usedOrders[i]) {
            cout << "Order " << i << " was not put into a batch" << endl;
            batches.push_back({static_cast<int>(i)});
        }
    }
*/
    return batches;
}