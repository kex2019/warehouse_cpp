#include "tabu.h"

#include <iostream>

vector<vector<int>> tabu::nswapgenerator::next() {
    if (isEof) {
        throw runtime_error("Tried to get next when nswapgenerator has reached eof");
    }

    while(true) {
        if(isEof) {
            return solution;
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

        if(tabus.isTabu(this->getMove())) {
            continue; // Keep going...
        }

        return solution;
    }
}

void tabu::nswapgenerator::doBestMove(int t) {
    tabus.insertTabu(t, getBestMove());
}
 
tuple<int,int,int,int> tabu::nswapgenerator::getBestMove() {
    int a, b;
    a = get<0>(bestMove) >= get<1>(bestMove) ? get<0>(bestMove) : get<1>(bestMove);
    b = get<0>(bestMove) < get<1>(bestMove) ? get<1>(bestMove) : get<0>(bestMove);
    int c, d;
    c = get<2>(bestMove) >= get<3>(bestMove) ? get<2>(bestMove) : get<3>(bestMove);
    d = get<2>(bestMove) < get<3>(bestMove) ? get<3>(bestMove) : get<2>(bestMove);
    return {a, b, c, d};
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

void tabu::nshiftgenerator::doBestMove(int t) {
    tabus.insertTabu(t, getBestMove());
}

tuple<int,int,int> tabu::nshiftgenerator::getBestMove() {
    int a, b;
    a = get<0>(bestMove) >= get<1>(bestMove) ? get<0>(bestMove) : get<1>(bestMove);
    b = get<0>(bestMove) < get<1>(bestMove) ? get<1>(bestMove) : get<0>(bestMove);
    return {a, b, solution[get<1>(bestMove)].size()};
}

tuple<int,int,int> tabu::nshiftgenerator::getMove() {
    int a, b;
    a = currentFirst >= currentSecond ? currentFirst : currentSecond;
    b = currentFirst < currentSecond ? currentSecond : currentFirst;
    return {a, b, solution[currentSecond].size()};
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

    while(true) {
        if(isEof) {
            return solution;
        }
        // TODO: Implement
        if(currentOrder >= solution[currentFirst].size()) {
            currentOrder = 0;
            currentSecond++;
        }
        if(currentSecond == currentFirst) {
            currentSecond++;
        }
        if(currentSecond >= solution.size()) {
            currentFirst++;
            currentSecond = 0;
            currentOrder = 0;
        }

        if(currentFirst == solution.size()) {
            isEof = true;
            return solution;
        }

        if(solution[currentSecond].size() >= robotCapacity) {
            // Can't add more orders here
            continue;
        }

        if(tabus.isTabu(this->getMove())) {
            continue;
        }

        vector<vector<int>> sol(solution);
        sol[currentSecond].push_back(solution[currentFirst][currentOrder]);
        return sol;
    }
}

vector<vector<int>> tabu::Tabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 10;
    int nOrders = warehouse.getPackageLocations().size();
    cw::cw c;
    auto solution = c.solve(nRobots, robotCapacity, warehouse);
    vector<vector<int>> bestSolution = solution;
    int bestSolutionScore = evaluateSolutionTime(warehouse, bestSolution, nRobots, robotCapacity);
    int t = 0;

    nswapgenerator gen(lifeTime, robotCapacity, solution);

    while(true) {
        t++;
        if(t >= 10000) {
            break;
        }

        gen.step(t); // Remove "old" tabu moves at this point
        gen.reset(solution);
        int nChecked = 0;
        int quality = evaluateSolutionTime(warehouse, solution, nRobots, robotCapacity);
        vector<vector<int>> nextBestSolution = solution;
        int nextBestQuality = quality;
        bool switched = false;
        int nSinceSwitch = 0;

        bool foundBetter = false;
        int nmovesCheck = 0;
        while(!gen.eof()) {
            if(switched && nSinceSwitch > nOrders) {
                break; // Checked enough since we found a better one.
            }
            nmovesCheck++;

            // Get next move to check
            auto next = gen.next();
            // Check it's not on tabu
            auto move = gen.getMove(); // Automagically checks the move is valid
            if(gen.eof()) {
                break;
            }

            nSinceSwitch++;
            int nextQuality = evaluateSolutionTime(warehouse, next, nRobots, robotCapacity);
            if(quality * 105 > nextQuality * 100) {
                if(switched && nextBestQuality > nextQuality) {
                    nextBestQuality = nextQuality;
                    nextBestSolution = next;
                    gen.setBestMove();
                } else if (!switched) {
                    switched = true;
                    nextBestQuality = nextQuality;
                    nextBestSolution = next;
                    foundBetter = true;
                    gen.setBestMove();
                }
            }
        }

        if(!foundBetter) {
            // Actually want to grab a tabu move, for now just kill everything
            break;
        }

        // Replace our solution with this one.
        gen.doBestMove(t);
        if(nextBestQuality < bestSolutionScore) {
            bestSolutionScore = nextBestQuality;
            bestSolution = nextBestSolution;
        }

        solution = nextBestSolution;
    }

//    cout << "FOUND: " << bestSolutionScore << endl;
    return bestSolution;
}