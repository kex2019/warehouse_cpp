#pragma once
#include <vector>
#include <utility>
#include <string>
using namespace std;
typedef pair<int,int> Position;
/*struct Position : public pair<int,int> {
    int& x() {
        return this->second;
    }
    int& y() {
        return this->first;
    }
};
*/

struct WarehouseInfo {
    int aisles = 3; // Number of aisles, the number of shelves in horizontal axis is = aisles*2
    int aisleWidth = 2;
    int shelfHeight = 1; // The size of a shelf in the vertical direction
    int crossAiles = 2; // Number of cross ailes
    int crossAilesWidth = 2;
    int packages = 10;
};

class Warehouse {
    int height;
    int width;
    Position drop;
    Position start;
    vector<vector<bool>> walkable;
    vector<Position> packageLocations;
    vector<vector<int>> pathLengthBetween; // path lengths between packages, starting at index 2. Index 0 and 1 are for start and drop
    vector<vector<int>> calcPathLengthFrom(Position a);
public:
    Warehouse(vector<vector<bool>> walkable, vector<Position> packageLocations);
    /*
        Expects this to index into packageLocation. (0 in idxSeq -> 0 in packageLocation)
        The sequence automaticaly starts at start and ends at drop.
        This is an ordered list, it doesn't find the optimal time for the "set" of packages.
        Finds the time for when we take the packages in this order
        idxSeq must contain at least one element 
    */
    int getLengthToStart(int a);
    int getLengthToDrop(int a);
    int getLengthBetween(int a, int b);
    int getTimeForSequence(const vector<int>& idxSeq) const;
    const vector<vector<bool>>& getWalkable() const;
    const vector<Position>& getPackageLocations() const;
    string to_string();
};

Warehouse generateRandomWarehouse(WarehouseInfo info);
