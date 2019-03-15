#include "warehouse.h"
#include <queue>
#include <iostream>
#include <random>
#include <set>
#include <algorithm>

Warehouse generateRandomWarehouse(WarehouseInfo info) {
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
    random_device dev;
    mt19937 rng(dev());
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
    for(int i = 0; i < packageLocations.size(); i++) {
        cout << packageLocations[i].first << ", " << packageLocations[i].second<< endl;
    }
    return Warehouse(walkable, packageLocations);
}

Warehouse::Warehouse(vector<vector<bool>> walkable, vector<Position> packageLocations) : walkable(walkable), packageLocations(packageLocations) {
    this->height = walkable.size();
    this->width = walkable[0].size();
    this->drop = {0,0};
    this->start = {0,0};
    auto fromdrop = calcPathLengthFrom(drop);
    auto fromstart = calcPathLengthFrom(start);
    vector<int> lenFromDrop(packageLocations.size() + 2);
    vector<int> lenFromStart(packageLocations.size() + 2);
    this->pathLengthBetween = vector<vector<int>>(packageLocations.size() + 2, vector<int>(packageLocations.size() + 2));
    for(int i = 0; i < packageLocations.size(); i++) {
        pathLengthBetween[i].resize(packageLocations.size() + 2);
        pathLengthBetween[1][i+2] = fromdrop[packageLocations[i].first][packageLocations[i].second];
        pathLengthBetween[0][i+2] = fromstart[packageLocations[i].first][packageLocations[i].second];

        auto fromPackage = calcPathLengthFrom(packageLocations[i]);
        pathLengthBetween[i+2][0] = lenFromStart[i+2];
        pathLengthBetween[i+2][1] = lenFromDrop[i+2];
        
        for(int j = 0; j < packageLocations.size(); j++) {
            pathLengthBetween[i+2][j+2] = fromPackage[packageLocations[i].first][packageLocations[i].second];
        }
    }
}

void push_neighbours(queue<Position>& q, Position a) {
    vector<Position> deltas{{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& p : deltas) {
        q.push(Position(a.first + p.first, a.second + p.second));
    }
}

vector<vector<int>> Warehouse::calcPathLengthFrom(Position a) {
    // Do BFS between a and b
    vector<vector<int>> lens(this->height, vector<int>(this->width, -1));
    queue<Position> q;
    lens[a.first][a.second] = -1;
    push_neighbours(q, a);
    q.push(a);
    while(!q.empty()) {
        auto cur = q.front();
        q.pop();
        if (cur.first < 0 || cur.first >= this->height || cur.second < 0 || cur.second >= this->width) {
            continue;
        }
        if(lens[cur.first][cur.second] != -1) {
            continue;
        }
        lens[cur.first][cur.second]++;
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


int Warehouse::getTimeForSequence(const vector<int> &idxSeq) const {
    // Calculate the time it takes to grab this sequence
    //static_assert(idxSeq.size() > 0, "idxSeq must have at least one element");
    int timeFromStart = pathLengthBetween[0][idxSeq[0]];
    int timeToEnd = pathLengthBetween[idxSeq.back()][1];
    int totalTime = 0;
    for(int i = 0; i < idxSeq.size() - 1; i++) {
        int timeBetween = pathLengthBetween[idxSeq[i]][idxSeq[i+1]];
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