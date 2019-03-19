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

tuple<int,int,int,int> tabu::nswapgenerator::getMove() {
    int a, b;
    a = currentFirst >= currentSecond ? currentFirst : currentSecond;
    b = currentFirst < currentSecond ? currentSecond : currentFirst;
    int c, d;
    c = currentFirstOrder >= currentSecondOrder ? currentFirstOrder : currentSecondOrder;
    d = currentFirstOrder < currentSecondOrder ? currentSecondOrder : currentFirstOrder;
    return {a, b, c, d};
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


vector<vector<int>> tabu::Tabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 10;
    int nOrders = warehouse.getPackageLocations().size();
    TabuList<tuple<int,int,int,int>> tabus(lifeTime);
    cw::cw c;
    auto solution = c.solve(nRobots, robotCapacity, warehouse);
    vector<vector<int>> bestSolution = solution;
    int bestSolutionScore = evaluateSolutionTime(warehouse, bestSolution, nRobots, robotCapacity);
    int t = 0;

//    cout << "INITIAL: " << bestSolutionScore << endl;
    
    while(true) {
        t++;
        if(t >= 10000) {
            break;
        }

        tabus.step(t); // Remove "old" tabu moves at this point
        nswapgenerator gen(solution);
        int nChecked = 0;
        int quality = evaluateSolutionTime(warehouse, solution, nRobots, robotCapacity);
        vector<vector<int>> nextBestSolution = solution;
        int nextBestQuality = quality;
        bool switched = false;
        int nSinceSwitch = 0;
        tuple<int,int,int,int> bestMove;

        bool foundBetter = false;
        while(!gen.eof()) {
            if(switched && nSinceSwitch > nOrders) {
                break; // Checked enough since we found a better one.
            }

            // Get next move to check
            auto next = gen.next();
            // Check it's not on tabu
            auto move = gen.getMove();
            if(tabus.isTabu(move)) {
                // Can't make this move, move on to the next one
                continue;
            }

            nSinceSwitch++;
            int nextQuality = evaluateSolutionTime(warehouse, next, nRobots, robotCapacity);
            if(quality * 105 > nextQuality * 100) {
                if(switched && nextBestQuality > nextQuality) {
                    nextBestQuality = nextQuality;
                    nextBestSolution = next;
                    bestMove = move;
                } else if (!switched) {
                    switched = true;
                    nextBestQuality = nextQuality;
                    nextBestSolution = next;
                    bestMove = move;
                    foundBetter = true;
                }
            }
        }

        if(!foundBetter) {
            // Actually want to grab a tabu move, for now just kill everything
            break;
        }

        // Replace our solution with this one.
        tabus.insertTabu(t, bestMove);
        if(nextBestQuality < bestSolutionScore) {
            bestSolutionScore = nextBestQuality;
            bestSolution = nextBestSolution;
        }

        solution = nextBestSolution;
    }

//    cout << "FOUND: " << bestSolutionScore << endl;
    return bestSolution;
}