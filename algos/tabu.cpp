#include "tabu.h"

#include <iostream>
#include <ctime>

pair<vector<vector<PackID>>, bool> tabu::nswapgenerator::next() {
    if (isEof) {
        throw runtime_error("Tried to get next when nswapgenerator has reached eof");
    }
    
    swap(solution[currentFirst][currentFirstOrder], solution[currentSecond][currentSecondOrder]);
//    int oldFirst = currentFirst, oldSecond = currentSecond, oldFirstOrder = currentFirstOrder, oldSecondOrder = currentSecondOrder;

    for(int i = 0; i < 50000; i++) {
        if(isEof) {
            return {solution, false};
        }


        // Swap back to the initial state
        currentFirst = rand() % solution.size();
        currentSecond = rand() % solution.size();
        if(currentFirst == currentSecond) {
            continue;
        }

        currentFirstOrder = rand() % solution[currentFirst].size();
        currentSecondOrder = rand() % solution[currentSecond].size();

        if(checked.find(getMove()) != checked.end()) {
            continue;
        }

        if(tabus.isTabu(this->getMove())) {
            checked.insert(getMove());
            continue; // Keep going...
        }

        swap(solution[currentFirst][currentFirstOrder], solution[currentSecond][currentSecondOrder]);
        return {solution, true};

/*        currentSecondOrder++;
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
            return {solution, false}; // Return initial solution
        }


        if(currentFirst == currentSecond) {
            cout << "Checking " << currentFirst << ", " << currentSecond << endl;
            throw runtime_error("xdxd");
        }
*/

        // Swap the orders, 
/*        swap(solution[currentFirst][currentFirstOrder], solution[currentSecond][currentSecondOrder]);
        if(currentFirst == solution.size() - 1 && currentSecond == solution.size() - 2 && currentFirstOrder == solution[currentFirst].size() - 1 && currentSecondOrder == solution[currentSecond].size()) {
            // This is the last order that will be processed
            isEof = true;
        }

        if(tabus.isTabu(this->getMove())) {
            continue; // Keep going...
        }
*/
//        return {solution, true};
    }

    isEof = true;
    return {solution, false};
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


pair<vector<vector<PackID>>, bool> tabu::nshiftgenerator::next() {
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
        vector<int> possibleSeconds;
        for(size_t i = 0; i < nRobots; i++) {
            if(i < solution.size() && solution[i].size() < robotCapacity) {
                possibleSeconds.push_back(i);
            } else if (i >= solution.size()){
                possibleSeconds.push_back(i);
            }
        }

        if(possibleSeconds.size() == 0) {
            isEof = true;
            return {solution, false};
        }

        currentSecond = possibleSeconds[rand() % possibleSeconds.size()];
        if(currentFirst >= solution.size() || solution[currentFirst].size() == 0) {
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
        vector<vector<PackID>> sol(solution);
        while(sol.size() <= currentSecond) {
            sol.push_back({});
        }
        sol[currentSecond].push_back(sol[currentFirst][currentOrder]);
        sol[currentFirst].erase(sol[currentFirst].begin() + currentOrder);
        return {sol, true};
    }

    return {{}, false};
}

tuple<int,int,int,int> tabu::BISwapper::moveToTuple(int fb, int sb, int fo, int so) {
    int a = fb > sb ? fb : sb;
    int b = a == fb ? sb : fb;
    int c = a == fb ? fo : so;
    int d = c == fo ? so : fo;
    return tuple<int,int,int,int>(a, b, c, d);
}

pair<int, vector<vector<PackID>>> tabu::BISwapper::doBestMove(int t, const Warehouse& warehouse, vector<vector<PackID>>& solution) {
    int bestFirstBatch = -1;
    int bestSecondBatch = -1;
    int bestFirstOrder = -1;
    int bestSecondOrder = -1;
    int bestIncrease = -1e6;
//    int fst = rand() % 2;
//    int m = 1;
//    int fst = 0;

    for(size_t i = 0; i < solution.size(); ++i) {
        if(!solution[i].size()) {
            continue;
        }
        int itime = warehouse.getTimeForSequence(solution[i]);
        for(size_t j = i+1; j < solution.size(); ++j) {
            if(!solution[j].size()) {
                continue;
            }
            int originalTime = itime + warehouse.getTimeForSequence(solution[j]);

            for(size_t io = 0; io < solution[i].size(); ++io) {
                for(size_t jo = 0; jo < solution[j].size(); ++jo) {
                    if(tabus.isTabu(moveToTuple(i,j,io,jo))) {
                        continue;
                    }

                    swap(solution[i][io], solution[j][jo]);
                    int newTime = warehouse.getTimeForSequence(solution[i]) + warehouse.getTimeForSequence(solution[j]);
                    if(originalTime - newTime > bestIncrease) {
                        bestIncrease = originalTime - newTime;
                        bestFirstBatch = i;
                        bestSecondBatch = j;
                        bestFirstOrder = io;
                        bestSecondOrder = jo;
                    }
                    swap(solution[i][io], solution[j][jo]);
                }
            }
        }
    }

    if(bestFirstBatch == -1) {
//        cout << "Could not find best increase...";
        return {bestIncrease, solution};
    }

    swap(solution[bestFirstBatch][bestFirstOrder], solution[bestSecondBatch][bestSecondOrder]);
    tabus.insertTabu(t, moveToTuple(bestFirstBatch, bestSecondBatch, bestFirstOrder, bestSecondOrder));
    return {bestIncrease, solution};
}

void tabu::BISwapper::step(int t) {
    tabus.step(t);
}



vector<vector<PackID>> tabu::Tabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 100;
    cw::cw c;
    auto sol = c.solve(nRobots, robotCapacity, warehouse);
    vector<SmallVector<PackID>> solution(sol.size());
    for(size_t i = 0; i < sol.size(); i++) {
        for(size_t j = 0; j < sol[i].size(); j++) {
            solution[i].push_back(sol[i][j]);
        }
    }

    vector<SmallVector<PackID>> bestSolution = solution;
    //vector<vector<int>> solution = sol;
    //vector<vector<int>> bestSolution = solution;


    int bestSolutionScore = evaluateSolutionTime(warehouse, bestSolution, nRobots, robotCapacity);
    int t = 0;

    BISwapperSmall swp(lifeTime, robotCapacity);
    clock_t avgclock = 0;
    int N = 0;
    clock_t totclock = clock();
//    cout << endl;
    this->stop.start();
    while(!this->stop.isDone(true)) {
        t++;
        clock_t start = clock();
        swp.step(t);
        int quality = evaluateSolutionTime(warehouse, solution, nRobots, robotCapacity);
        vector<SmallVector<PackID>> nextBestSolution = solution;
        int nextBestQuality = quality;
        // Replace our solution with this one.
        auto moveres = swp.doBestMove(t, warehouse, solution);

        nextBestQuality = quality - moveres.first;
        nextBestSolution = moveres.second;
        if(nextBestQuality < bestSolutionScore) {
//            cout << "NextBest: " << nextBestQuality << endl;
            bestSolutionScore = nextBestQuality;
            bestSolution = nextBestSolution;
        }

        solution = nextBestSolution;
        avgclock = (avgclock * N + (clock() - start)) / (N + 1);
        N++;
    }

    double avgms = (double)avgclock / (double)CLOCKS_PER_SEC;
    cout << "TOOK an avarage of: " << avgms << " to process, total: " << ((double)(clock() - totclock)/ (double)CLOCKS_PER_SEC) << endl;

    vector<vector<PackID>> convertedVec(sol.size());
    for(size_t i = 0; i < bestSolution.size(); i++) {
        for(size_t j = 0; j < bestSolution[i].size(); j++) {
            convertedVec[i].push_back(bestSolution[i][j]);
        }
    }

    return convertedVec;
}









tuple<int,int,int> tabu::BISwapperSmall::movePushToTuple(int fb, int sb, int o) {
    return tuple<int,int,int>(fb, sb, o);
}

tuple<int,int,int,int> tabu::BISwapperSmall::moveToTuple(int fb, int sb, int fo, int so) {
    int a = fb > sb ? fb : sb;
    int b = a == fb ? sb : fb;
    int c = a == fb ? fo : so;
    int d = c == fo ? so : fo;
    return tuple<int,int,int,int>(a, b, c, d);
}

pair<int, vector<SmallVector<PackID>>> tabu::BISwapperSmall::doBestMove(int t, const Warehouse& warehouse, vector<SmallVector<PackID>>& solution) {
    int bestFirstBatch = -1;
    int bestSecondBatch = -1;
    int bestFirstOrder = -1;
    int bestSecondOrder = -1;
    int bestIncrease = -1e6;
//    int fst = rand() % 2;
//    int m = 1;
//    int fst = 0;

    for(size_t i = 0; i < solution.size(); ++i) {
        if(!solution[i].size()) {
            continue;
        }
        int itime = warehouse.getTimeForSequence(solution[i]);
        for(size_t j = i+1; j < solution.size(); ++j) {
            if(!solution[j].size()) {
                continue;
            }
            int originalTime = itime + warehouse.getTimeForSequence(solution[j]);

            for(size_t io = 0; io < solution[i].size(); ++io) {
                for(size_t jo = 0; jo < solution[j].size(); ++jo) {
                    if(tabus.isTabu(moveToTuple(i,j,io,jo))) {
                        continue;
                    }

                    swap(solution[i][io], solution[j][jo]);
                    int newTime = warehouse.getTimeForSequence(solution[i]) + warehouse.getTimeForSequence(solution[j]);
                    if(originalTime - newTime > bestIncrease) {
                        bestIncrease = originalTime - newTime;
                        bestFirstBatch = i;
                        bestSecondBatch = j;
                        bestFirstOrder = io;
                        bestSecondOrder = jo;
                    }
                    swap(solution[i][io], solution[j][jo]);
                }
            }
        }
    }

    int bestFirstBatchPush = -1;
    int bestSecondBatchPush = -1;
    int bestOrderPush = -1;
    for(size_t i = 0; i < solution.size(); ++i) {
        if(!solution[i].size()) {
            continue;
        }

        int ot1 = warehouse.getTimeForSequence(solution[i]);
        for(size_t j = 0; j < solution.size(); j++) {
            if(i == j) {
                continue;
            }

            if(static_cast<int>(solution[j].size()) < this->capacity) {
                int ot2 = 0;
                if(solution[j].size() > 0) {
                    ot2 = warehouse.getTimeForSequence(solution[j]);
                }

                // Push from i to j
                for(size_t k = 0; k < solution[i].size(); k++) {
                    if(pushTabus.isTabu(movePushToTuple(i,j,k))) {
                        continue;
                    }

                    // So reconstruct the two things
                    SmallVector<PackID> newj = solution[j];
                    newj.push_back(solution[i][k]);
                    SmallVector<PackID> newi;
                    for(size_t idx = 0; idx < solution[i].size(); idx++) {
                        if(idx != k) {
                            newi.push_back(solution[i][idx]);
                        }
                    }

                    int nti = 0;
                    if(newi.size() > 0) {
                        nti = warehouse.getTimeForSequence(newi);
                    }

                    int ntj = warehouse.getTimeForSequence(newj);
                    int increase = (ot1 + ot2) - (nti + ntj);
                    if(increase > bestIncrease) {
                        bestIncrease = increase;
                        bestFirstBatchPush = i;
                        bestSecondBatchPush = j;
                        bestOrderPush = k;
                        bestFirstBatch = -1;
                        bestSecondBatch = -1;
                    }
                }
            }
        }
    }


    if(bestFirstBatch == -1 && bestFirstBatchPush == -1) {
//        cout << "Could not find best increase...";
        return {bestIncrease, solution};
    }

    if(bestFirstBatchPush == -1){
        swap(solution[bestFirstBatch][bestFirstOrder], solution[bestSecondBatch][bestSecondOrder]);
        tabus.insertTabu(t, moveToTuple(bestFirstBatch, bestSecondBatch, bestFirstOrder, bestSecondOrder));
        return {bestIncrease, solution};
    } else {
        auto c = solution[bestFirstBatchPush][bestOrderPush];
        solution[bestSecondBatchPush].push_back(c);

        SmallVector<PackID> newi;
        for(size_t i = 0; i < solution[bestFirstBatchPush].size(); i++) {
            if(static_cast<int>(i) != bestOrderPush) {
                newi.push_back(solution[bestFirstBatchPush][i]);
            }
        }
        solution[bestFirstBatchPush] = newi;

        pushTabus.insertTabu(t, movePushToTuple(bestFirstBatchPush, bestSecondBatchPush, bestSecondOrder));
        pushTabus.insertTabu(t, movePushToTuple(bestSecondBatchPush, bestFirstBatchPush, solution[bestSecondBatchPush].size() - 1));        
        return {bestIncrease, solution};
    }
}

void tabu::BISwapperSmall::step(int t) {
    tabus.step(t);
}


vector<vector<PackID>> tabu::OldSwapGenerator::next() {
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

tuple<int,int,int,int> tabu::OldSwapGenerator::getMove() {
    int a, b;
    a = currentFirst >= currentSecond ? currentFirst : currentSecond;
    b = currentFirst < currentSecond ? currentSecond : currentFirst;
    int c, d;
    c = currentFirstOrder >= currentSecondOrder ? currentFirstOrder : currentSecondOrder;
    d = currentFirstOrder < currentSecondOrder ? currentSecondOrder : currentFirstOrder;
    return {a, b, c, d};
}

bool tabu::OldSwapGenerator::eof() {
    return isEof;
}


vector<vector<PackID>> tabu::OldTabu::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
    int lifeTime = 10;
    int nOrders = warehouse.getPackageLocations().size();
    TabuList<tuple<int,int,int,int>> tabus(lifeTime);
    cw::cw c;
    auto solution = c.solve(nRobots, robotCapacity, warehouse);
    vector<vector<PackID>> bestSolution = solution;
    int bestSolutionScore = evaluateSolutionTime(warehouse, bestSolution, nRobots, robotCapacity);
    int t = 0;
    
    this->stop.start();
    while(!this->stop.isDone(true)) {
        t++;
        tabus.step(t);
        OldSwapGenerator gen(solution);
        int quality = evaluateSolutionTime(warehouse, solution, nRobots, robotCapacity);
        vector<vector<PackID>> nextBestSolution = solution;
        int nextBestQuality = quality;
        bool switched = false;
        int nSinceSwitch = 0;
        int nCheckedN = 0;
        tuple<int,int,int,int> bestMove;

        bool foundBetter = false;
        while(!gen.eof()) {
            if(switched && nSinceSwitch > nOrders && nCheckedN > 4 * nOrders) {
                break; // Checked enough since we found a better one.
            }
            nCheckedN++;

            auto next = gen.next();
            // Check it's not on tabu
            auto move = gen.getMove();
            if(tabus.isTabu(move)) {
                // Can't make this move, move on to the next one
                continue;
            }

            if(switched) {
                nSinceSwitch++;

            }
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

    return bestSolution;
}