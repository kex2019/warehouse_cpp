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


        if(currentFirst == currentSecond) {
            cout << "Checking " << currentFirst << ", " << currentSecond << endl;
            throw runtime_error("xdxd");
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
    a = get<0>(bestMove) < get<1>(bestMove) ? get<0>(bestMove) : get<1>(bestMove);
    b = a == get<0>(bestMove) ? get<1>(bestMove) : get<0>(bestMove);
    int c, d;
    c = get<2>(bestMove) < get<3>(bestMove) ? get<2>(bestMove) : get<3>(bestMove);
    d = c == get<2>(bestMove) ? get<3>(bestMove) : get<2>(bestMove);
    return {a, b, c, d};
}

tuple<int,int,int,int> tabu::nswapgenerator::getMove() {
    int a, b;
    a = currentFirst < currentSecond ? currentFirst : currentSecond;
    b = a == static_cast<int>(currentFirst) ? currentSecond : currentFirst;
    int c, d;
    c = currentFirstOrder < currentSecondOrder ? currentFirstOrder : currentSecondOrder;
    d = c == static_cast<int>(currentFirstOrder) ? currentSecondOrder : currentFirstOrder;
    return {a, b, c, d};
}

void tabu::nshiftgenerator::doBestMove(int t) {
    tabus.insertTabu(t, getBestMove());
}

tuple<int,int,int> tabu::nshiftgenerator::getBestMove() {
    int a, b;
    a = get<0>(bestMove) < get<1>(bestMove) ? get<0>(bestMove) : get<1>(bestMove);
    b = a == get<0>(bestMove) ? get<1>(bestMove) : get<0>(bestMove);
    return {a, b, solution[get<1>(bestMove)].size()};
}

tuple<int,int,int> tabu::nshiftgenerator::getMove() {
    int a, b;
    a = currentFirst < currentSecond ? currentFirst : currentSecond;
    b = a == static_cast<int>(currentFirst) ? currentSecond : currentFirst;
    return {a, b, solution[currentSecond].size()};
}

bool tabu::nswapgenerator::eof() {
    return isEof;
}

bool tabu::nshiftgenerator::eof() {
    return isEof;
}


pair<vector<vector<int>>, bool> tabu::nshiftgenerator::next() {
    if (isEof) {
        throw runtime_error("Tried to get next when nshiftgenerator has reached eof");
    }

    int i = 0;
    int _max = 10000;
    while(true) {
        i++;
        if(i >= _max) {
            break;
        }

        currentFirst = rand() % solution.size();
        currentSecond = rand() % nRobots;
        if(currentFirst >= solution.size() || solution[currentFirst].size() == 0 || (currentSecond < solution.size() && solution[currentSecond].size() >= robotCapacity)) {
            continue;
        }

        currentOrder = rand() % solution[currentFirst].size();
        auto move = getMove();
        if(checked.find(move) != checked.end()) {
            // Do something else that's actually smart here...
            continue;
        }

        if(tabus.isTabu(move)) {
            // Move is tabu :'(
            checked.insert(move); // Might not want to do this, maybe is enough to have it in tabu list
            //cout << "Move is tabu" << endl;
            continue;
        }

        // We can do this move :o 
        vector<vector<int>> sol(solution);
        while(sol.size() <= currentSecond) {
            sol.push_back({});
        }
        sol[currentSecond].push_back(sol[currentFirst][currentOrder]);
        sol[currentFirst].erase(sol[currentFirst].begin() + currentOrder);
        return {sol, true};
    }

    return {{}, false};
}

vector<vector<int>> tabu::Tabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 10;
    int nOrders = warehouse.getPackageLocations().size();
    cw::cw c;
    auto solution = c.solve(nRobots, robotCapacity, warehouse);
    vector<vector<int>> bestSolution = solution;
    int bestSolutionScore = evaluateSolutionTime(warehouse, bestSolution, nRobots, robotCapacity);
    int t = 0;
    int maxN = 5 * nOrders;

    nshiftgenerator gen(lifeTime, nRobots, robotCapacity, solution);
    cout << endl;
    while(true) {
        t++;
        if(t >= 10000) {
            break;
        }
        cout << " T:             " << t << "       \r";
        cout.flush();

        gen.step(t); // Remove "old" tabu moves at this point
        gen.reset(solution);
        int quality = evaluateSolutionTime(warehouse, solution, nRobots, robotCapacity);
        vector<vector<int>> nextBestSolution = solution;
        int nextBestQuality = quality;
        bool switched = false;
        int nSinceSwitch = 0;

        bool foundBetter = false;
        int tt = 0;
        while(!gen.eof()) {
            tt++;
            if(switched && nSinceSwitch > nOrders) {
                break; // Checked enough since we found a better one.
            }
            if(tt > maxN) {
                break; // Checked to many
            }


            // Get next move to check
            auto nexts = gen.next();
            if(!nexts.second) {
                return bestSolution;
            }

            auto next = nexts.first;
            // Check it's not on tabu
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

    return bestSolution;
}