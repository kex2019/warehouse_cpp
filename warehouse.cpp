#include "warehouse.h"
#include <queue>
#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>

template<typename T>
void inlineCerrVec(const vector<T>& vec) {
    cerr << "{ ";
    for(auto i : vec ) {
        cerr << i << " ";
    }
    cerr << "}";
}
template<typename T>
void inlineCoutVec(const vector<T>& vec) {
    cerr << "{ ";
    for(auto i : vec ) {
        cerr << i << " ";
    }
    cerr << "}";
}

int evaluateSolutionTime(const Warehouse & warehouse, const vector<PackID>& batches, size_t nRobots, size_t robotCapacity) {
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;

    int nBatches = batches.size() / robotCapacity;
    if(batches.size() % robotCapacity != 0) {
        nBatches++;
    }

    for(int i = 0; i < nBatches; i++) {
        int from = i * robotCapacity;
        int to = std::min((i + 1) * robotCapacity, batches.size());
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

int evaluateSolutionTime(const Warehouse & warehouse, const vector<SmallVector<PackID>>& batches, size_t nRobots, size_t robotCapacity) {
    vector<PackID> takenPackages(warehouse.getPackageLocations().size(), 0);
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;
    bool invalid = false;


    for(size_t i = 0; i < batches.size(); i++) {
        if(batches[i].size() == 0) {
            continue; // No items for this robot :'(
        }
        if(batches[i].size() > robotCapacity) {
            cerr << "Batch: ";
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
    }

    return *latestRobots.begin();

}

int evaluateSolutionTime(const Warehouse & warehouse, const vector<vector<PackID>>& batches, size_t nRobots, size_t robotCapacity) {
    vector<PackID> takenPackages(warehouse.getPackageLocations().size(), 0);
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;
    bool invalid = false;

    for(size_t i = 0; i < batches.size(); i++) {
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
        for(auto j : batches[i]) {
            takenPackages[j]++;
        }
    }

    for(size_t i = 0; i < takenPackages.size(); i++) {
        if(takenPackages[i] == 0) {
            cerr << "Package " << i << " was not taken" << endl;
            invalid = true;
        }
        if(takenPackages[i] > 1) {
            cerr << "Package " << i << " was taken " << takenPackages[i] << " times" << endl;
            invalid = true;
        }
    }

    if(invalid) {
        throw runtime_error("The solution was invalid, check stderr for reason");
    }
    
    return *latestRobots.begin();
}

vector<int> getRobotTravelTimes(const Warehouse& warehouse, const vector<vector<PackID>>& batches, size_t nRobots, size_t robotCapacity) {
    vector<PackID> takenPackages(warehouse.getPackageLocations().size(), 0);
    set<int> earliestRobots;
    set<int, greater<>> latestRobots;
    bool invalid = false;

    for(size_t i = 0; i < batches.size(); i++) {
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
        for(size_t j : batches[i]) {
            takenPackages[j]++;
        }
    }

    for(size_t i = 0; i < takenPackages.size(); i++) {
        if(takenPackages.at(i) == 0) {
            cerr << "Package " << i << " was not taken" << endl;
            invalid = true;
        }
        if(takenPackages[i] > 1) {
            cerr << "Package " << i << " was taken " << takenPackages[i] << " times" << endl;
            invalid = true;
        }
    }

    if(invalid) {
        throw runtime_error("The solution was invalid, check stderr for reason");
    }

    vector<int> results(latestRobots.begin(), latestRobots.end());    
    return results;
}

Warehouse generateRandomWarehouse(WarehouseInfo info, long seed) {
    if (info.crossAilesWidth < 2) {
        info.crossAilesWidth = 2; // We must be at least 2 wide to be able to drop things off
    }
    // First calculate the x positions for every shelf
    info.aisleWidth += 2; // The shelves take up 2 spots as well
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
    for(size_t i = 0; i < packageLocations.size(); i++) {
        pathLengthBetween[i].resize(packageLocations.size() + 2);
        pathLengthBetween[1][i+2] = fromdrop[packageLocations[i].first][packageLocations[i].second];
        pathLengthBetween[0][i+2] = fromstart[packageLocations[i].first][packageLocations[i].second];

        auto fromPackage = calcPathLengthFrom(packageLocations[i]);
        pathLengthBetween[i+2][0] = fromstart[packageLocations[i].first][packageLocations[i].second];
        pathLengthBetween[i+2][1] = fromdrop[packageLocations[i].first][packageLocations[i].second];
        
        for(size_t j = 0; j < packageLocations.size(); j++) {
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

int Warehouse::getLengthToStart(PackID a) {
    return pathLengthBetween[1][a+2];
}
int Warehouse::getLengthToDrop(PackID a) {
    return pathLengthBetween[0][a+2];
}
int Warehouse::getLengthBetween(PackID a, PackID b) {
    return pathLengthBetween[a+2][b+2];
}
const vector<vector<int>>& Warehouse::getPathLengths() const {
    return this->pathLengthBetween;
}

int findFirstNonNegOneForward(const vector<PackID>& idxSeq, int i, int to) {
    while(i < to && idxSeq[i] == -1) {
        i++;
    }
    return i;
}
int findFirstNonNegOneBackward(const vector<PackID>& idxSeq, int i, int from) {
    while(i > from && idxSeq[i] == -1) {
        i--;
    }
    return i;
}

int Warehouse::getTimeForSequence(const SmallVector<PackID> &idxSeq) const {
    if(idxSeq.size() <= 0) {
        throw runtime_error("idxSeq must have at least one element");
    }

    int totalTime = 0;
    for(int i = 0; i < idxSeq.size() - 1; i++) {
        totalTime += pathLengthBetween[idxSeq[i] + 2][idxSeq[i + 1] + 2];
    }

    return totalTime + pathLengthBetween[0][idxSeq.front() + 2] + pathLengthBetween[idxSeq.back() + 2][1];
}


int Warehouse::getTimeForSequence(const vector<PackID> &idxSeq, int from, int to) const {
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

    int start = findFirstNonNegOneForward(idxSeq, from, to);
    if(start == to) {
        // This means that the entire sequence doesn't contain any orders
        return 0;
    }
    int end = findFirstNonNegOneBackward(idxSeq, to - 1, from);
    if(start > end || start >= (int)idxSeq.size() || end >= (int)idxSeq.size()) {
        // This is an invalid sequence
        cerr << "FROM: " << from << ", TO: " << to << ", START: " << start << ", END: " << end << endl;
        throw runtime_error("Tried to get time for sequence " + std::to_string(start) + string(" to ") + std::to_string(end) + string(" which is an invalid sequence"));
    }

    int timeFromStart = pathLengthBetween[0][idxSeq[start] + 2];
    int timeToEnd = pathLengthBetween[idxSeq[end] + 2][1];
    int totalTime = 0;
    for(int i = from; i < to - 1; i++) {
        int next = i + 1;
        if(idxSeq[i] == -1) {
            continue; // This is a minus one, just keep on going
        }

        next = findFirstNonNegOneForward(idxSeq, next, to);
        if(next >= to - 1) {
            break; // This grabs no more orders
        }
        
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
    for(size_t y = 0; y < walkable.size(); y++) {
        for(size_t x = 0; x < walkable[y].size(); x++) {
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
