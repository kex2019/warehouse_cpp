#include "warehouse.h"
#include <queue>
#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>

void inlineCerrVec(const vector<int>& vec) {
    cerr << "{ ";
    for(auto i : vec ) {
        cerr << i << " ";
    }
    cerr << "}";
}

void inlineCoutVec(const vector<int>& vec) {
    cerr << "{ ";
    for(auto i : vec ) {
        cerr << i << " ";
    }
    cerr << "}";
}

int evaluateSolutionTime(const Warehouse & warehouse, const vector<int>& batches, int nRobots, int robotCapacity) {
    vector<bool> takenPackages(warehouse.getPackageLocations().size(), false);
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;
    bool invalid = false;

    int nBatches = batches.size() / robotCapacity;
    if(batches.size() % robotCapacity != 0) {
        nBatches++;
    }

    inlineCoutVec(batches);

    for(int i = 0; i < nBatches; i++) {
        int from = i * robotCapacity;
        int to = std::min((i + 1) * robotCapacity, (int)batches.size());
        int tseq = warehouse.getTimeForSequence(batches, from, to);
        if(nRobots > earliestRobots.size()) {
            earliestRobots.insert(tseq);
            latestRobots.insert(tseq);
        } else {
            // Remove the earliest robot
            int earliest = *earliestRobots.begin();
            earliestRobots.erase(earliestRobots.begin());
            earliestRobots.insert(tseq + earliest);
            latestRobots.insert(tseq + earliest);
        }
    }

    
    return *latestRobots.begin();
}

int evaluateSolutionTime(const Warehouse & warehouse, const vector<vector<int>>& batches, int nRobots, int robotCapacity) {
    vector<bool> takenPackages(warehouse.getPackageLocations().size(), false);
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;
    bool invalid = false;

    for(int i = 0; i < batches.size(); i++) {
        if(batches[i].size() == 0) {
            continue; // No items for this robot :'(
        }
        if(batches[i].size() > robotCapacity) {
            cerr << "Batch: ";
            inlineCerrVec(batches[i]);
            cerr << " has size: " << batches[i].size() << " while the robot can only carry: " << robotCapacity << endl;
            invalid = true;
        }
        int tseq = warehouse.getTimeForSequence(batches[i]);
        if(nRobots > earliestRobots.size()) {
            earliestRobots.insert(tseq);
            latestRobots.insert(tseq);
        } else {
            // Remove the earliest robot
            int earliest = *earliestRobots.begin();
            earliestRobots.erase(earliestRobots.begin());
            earliestRobots.insert(tseq + earliest);
            latestRobots.insert(tseq + earliest);
        }
        for(int j : batches[i]) {
            takenPackages[j] = true;
        }
    }

    for(int i = 0; i < takenPackages.size(); i++) {
        if(!takenPackages[i]) {
            cerr << "Package " << i << " was not taken" << endl;
            invalid = true;
        }
    }

    if(invalid) {
        throw runtime_error("The solution was invalid, check stderr for reason");
    }
    
    return *latestRobots.begin();
}


Warehouse generateRandomWarehouse(WarehouseInfo info, long seed) {
    if (info.crossAilesWidth < 2) {
        info.crossAilesWidth = 2; // We must be at least 2 wide to be able to drop things off
    }
    // First calculate the x positions for every shelf
    info.aisleWidth += 2; // The shelves take up 2 spots as well
    int nShelves = info.aisles*2;
    vector<int> shelfX(info.aisles*2);
    for(int i = 0; i < info.aisles; i++) {
        shelfX[2*i] = i * info.aisleWidth;
        shelfX[2*i + 1] = i * info.aisleWidth + info.aisleWidth - 1;
    }

    int maxX = info.aisleWidth * (info.aisles - 1) + info.aisleWidth;
    vector<int> shelfY(info.crossAiles + 1); // Top coordinate of the shelf
    for (int i = 0; i < info.crossAiles + 1; i++) {
        shelfY[i] = info.shelfHeight * i + info.crossAilesWidth * (i + 1);
    }

    int maxY = info.shelfHeight * (info.crossAiles) + info.crossAilesWidth * (info.crossAiles + 1) + info.shelfHeight + info.crossAilesWidth; // One more crossaisle at the end as well
    //Not just position all of them
    vector<vector<bool>> walkable(maxY, vector<bool>(maxX, true));
    for(int yidx = 0; yidx < info.crossAiles + 1; yidx++) {
        for(int xidx = 0; xidx < info.aisles * 2; xidx++) {
            for(int i = 0; i < info.shelfHeight; i++) {
                walkable[shelfY[yidx]+i][shelfX[xidx]] = false;
            }
        }
    }
//    random_device dev;

    mt19937 rng(seed);
    uniform_int_distribution<std::mt19937::result_type> widthDist(0,info.aisles*2-1);
    uniform_int_distribution<std::mt19937::result_type> heightDist(0,info.crossAiles);
    uniform_int_distribution<std::mt19937::result_type> heightAddDist(0,info.shelfHeight-1);

    vector<Position> packageLocations(info.packages);
    set<Position> usedPositions;
    for(int i = 0; i < info.packages; i++) {
        int x,y;
        do {
            int xidx = widthDist(rng);
            int yidx = heightDist(rng);
            x = shelfX[xidx];
            y = shelfY[yidx] + heightAddDist(rng);
        }while(usedPositions.find({y,x}) != usedPositions.end());
        packageLocations[i] = {y,x};
    }
//    for(int i = 0; i < packageLocations.size(); i++) {
//        cout << packageLocations[i].first << ", " << packageLocations[i].second<< endl;
//    }
    return Warehouse(walkable, packageLocations);
}

Warehouse::Warehouse(vector<vector<bool>> walkable, vector<Position> packageLocations) : walkable(walkable), packageLocations(packageLocations) {
    this->height = walkable.size();
    this->width = walkable[0].size();
    this->drop = {0,0};
    this->start = {0,0};
    auto fromdrop = calcPathLengthFrom(drop);
    auto fromstart = calcPathLengthFrom(start);
    this->pathLengthBetween = vector<vector<int>>(packageLocations.size() + 2, vector<int>(packageLocations.size() + 2));
    for(int i = 0; i < packageLocations.size(); i++) {
        pathLengthBetween[i].resize(packageLocations.size() + 2);
        pathLengthBetween[1][i+2] = fromdrop[packageLocations[i].first][packageLocations[i].second];
        pathLengthBetween[0][i+2] = fromstart[packageLocations[i].first][packageLocations[i].second];

        auto fromPackage = calcPathLengthFrom(packageLocations[i]);
        pathLengthBetween[i+2][0] = fromstart[packageLocations[i].first][packageLocations[i].second];
        pathLengthBetween[i+2][1] = fromdrop[packageLocations[i].first][packageLocations[i].second];
        
        for(int j = 0; j < packageLocations.size(); j++) {
            pathLengthBetween[i+2][j+2] = fromPackage[packageLocations[j].first][packageLocations[j].second];
        }
    }
}

void push_neighbours(queue<pair<Position,Position>>& q, Position a) {
    vector<Position> deltas{{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& p : deltas) {
        q.push({Position(a.first + p.first, a.second + p.second), a});
    }
}

vector<vector<int>> Warehouse::calcPathLengthFrom(Position a) {
    // Do BFS between a and b
    vector<vector<int>> lens(this->height, vector<int>(this->width, -1));
    queue<pair<Position,Position>> q;
    lens[a.first][a.second] = 0;
    push_neighbours(q, a);
    while(!q.empty()) {
        auto top = q.front();
        auto cur = top.first;
        auto prev = top.second;
        q.pop();
        if (cur.first < 0 || cur.first >= this->height || cur.second < 0 || cur.second >= this->width) {
            continue;
        }
        if(lens[cur.first][cur.second] != -1) {
            continue;
        }
        lens[cur.first][cur.second] = lens[prev.first][prev.second] + 1;
        push_neighbours(q, cur);
    }

    return lens;
}

int Warehouse::getLengthToStart(int a) {
    return pathLengthBetween[1][a+2];
}
int Warehouse::getLengthToDrop(int a) {
    return pathLengthBetween[0][a+2];
}
int Warehouse::getLengthBetween(int a, int b) {
    return pathLengthBetween[a+2][b+2];
}
const vector<vector<int>>& Warehouse::getPathLengths() const {
    return this->pathLengthBetween;
}

int findFirstNonNegOne(const vector<int>& idxSeq, int i, int to, int add = 1) {
    while(i < to && idxSeq[i] == -1) {
        i+= add;
    }
    return i;
}

int Warehouse::getTimeForSequence(const vector<int> &idxSeq, int from, int to) const {
    cout << "FROM/TO/SIZE:" << from << "; " << to << ";" << idxSeq.size() << endl;
    if(from == -1) {
        from = 0;
    }
    if(to == -1) {
        to = idxSeq.size();
    }
    // Calculate the time it takes to grab this sequence
    if(idxSeq.size() <= 0) {
        throw runtime_error("idSeq must have at least one element");
    }

    int start = findFirstNonNegOne(idxSeq, 0, to);
    int end = findFirstNonNegOne(idxSeq, to - 1, -1);
    int timeFromStart = pathLengthBetween[0][idxSeq[start] + 2];
    int timeToEnd = pathLengthBetween[idxSeq[end] + 2][1];
//    inlineCoutVec(idxSeq);
//    cout << ":: TimeStart: " << timeFromStart << ", timeEnd: " << timeToEnd << endl;
    int totalTime = 0;
    for(int i = from; i < to - 1; i++) {
        int next = i + 1;
        if(idxSeq[i] == -1) {
            continue; // This is a minus one, just keep on going
        }
        next = findFirstNonNegOne(idxSeq, next, to - 1);
        if(next >= to - 1) {
            break; // This grabs no more orders
        }
        
        cout << "I:" << i << " NEXT:" << next << " ### ";
        cout.flush();
        cout << idxSeq[i] << ", " << idxSeq[next] << endl;
        cout << pathLengthBetween.size() << endl;
        int timeBetween = pathLengthBetween[idxSeq[i] + 2][idxSeq[next] + 2];
        totalTime += timeBetween;
    }
    return totalTime + timeFromStart + timeToEnd;
}

const vector<vector<bool>>& Warehouse::getWalkable() const{
    return this->walkable;
}

const vector<Position>& Warehouse::getPackageLocations() const {
    return this->packageLocations;
}

string Warehouse::to_string() {
    string ret;
    for(int y = 0; y < walkable.size(); y++) {
        for(int x = 0; x < walkable[y].size(); x++) {
            auto walk= walkable[y][x];
            if(find(packageLocations.begin(), packageLocations.end(), Position(y,x)) != packageLocations.end()) {
                ret += '2';
            } else {
                ret += (walk ? '1' : '0');
            }
        }
        ret += '\n';
    }
    return ret;
}
