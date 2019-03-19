#include "tabu.h"

#include <iostream>

vector<vector<int>> tabu::nswapgenerator::next() {
    if (isEof) {
        throw runtime_error("Tried to get next when nswapgenerator has reached eof");
    }

    // Swap back to the initial state
    swap(solution[currentFirst][currentFirstOrder], solution[currentSecond][currentSecondOrder]);
    currentSecondOrder++;
    if(currentSecondOrder == solution[currentSecond].size()) {
        currentFirstOrder++;
        currentSecondOrder = 0;
    }
    if(currentFirstOrder == solution[currentFirst].size()){
        // We want to go to the next second element
        currentSecond++;
        currentFirstOrder = 0;
        currentSecondOrder = 0;
    }
    if(currentSecond == currentFirst) {
        currentSecond++;
    }
    if(currentSecond >= solution.size()) {
        currentFirst++;
        currentSecond = 0;
    }
    if(currentFirst >= solution.size()) {
        isEof = true;
        return solution; // Return initial solution
    }

    // Swap the orders, 
    swap(solution[currentFirst][currentFirstOrder], solution[currentSecond][currentSecondOrder]);
    if(currentFirst == solution.size() - 1 && currentSecond == solution.size() - 2 && currentFirstOrder == solution[currentFirst].size() - 1 && currentSecondOrder == solution[currentSecond].size()) {
        // This is the last order that will be processed
        isEof = true;
    }

    return solution;
}
vector<tuple<int,int,int,int>> getMove() {
    return 
}

bool tabu::nswapgenerator::eof() {
    return isEof;
}

bool tabu::nshiftgenerator::eof() {
    return isEof;
}

vector<vector<int>> tabu::nshiftgenerator::next() {
    if (isEof) {
        throw runtime_error("Tried to get next when nshiftgenerator has reached eof");
    }

    // TODO: Implement

    if(currentSecond == currentFirst) {
        currentSecond++;
    }
    if(currentSecond >= solution.size()) {
        currentFirst++;
        currentSecond = 0;
    }
    return solution;
}


vector<vector<int>> tabu::tabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 10;
    TabuList<tuple<int,int,int,int>> tabus(lifeTime);

}