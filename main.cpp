#include <iostream>
#include <tuple>
#include <algorithm>
#include <random>
#include <string>
#include <fstream>
#include <ctime>
#include "warehouse.h"
#include "algos/cw.h"
#include "algos/greedy.h"
#include "algos/ga.h"
#include "algos/tabu.h"
#include "algos/complsearch.h"


vector<long> generateSeeds(int N) {
    random_device dev;
    vector<long> seeds(N);
    for(int i = 0; i < N; i++) {
        seeds[i] = dev();
    }
    return seeds;
}

int resultId = 0;
int nextId() {
    return resultId++;
}
int groupId = 0;
int nextGroupId() {
    return groupId++;
}

void resetGroupId() {
    groupId = 0;
}

class ResultHandler {
    string name;
    ofstream resultFile;
    ofstream robotResultFile;
public: 
    ResultHandler(string dir, std::string name, bool overwrite = false) : name(name) {
        auto flags = ios::out;
        if(overwrite)
            flags = flags | ios::trunc;
        resultFile.open(dir + "/" + name + ".results", flags);
        if(!resultFile.is_open()) {
            cerr << "Could not open " << (dir + "/" + name + ".results") << " for writing results, throwing" << endl;
            throw runtime_error("Could not open result file"); 
        }

        robotResultFile.open(dir + "/" + name + "_robots.results", flags);
        if(!robotResultFile.is_open()) {
            cerr << "Could not open " << (dir + "/" + name + "_robots.results") << " for writing results, throwing" << endl;
            throw runtime_error("Could not open result file"); 
        }
    }
    ~ResultHandler() {
        resultFile.close();
        robotResultFile.close();
    }
    void appendResult(int group, int result, int nRobots, int robotCapacity, int nPackages, int seed, long millis, const vector<int> &robotTravelTimes) {
        int id = nextId();
        resultFile << id << "," << group << "," << result << "," << robotTravelTimes.size() << "," << nRobots << "," << robotCapacity << "," << nPackages << "," << seed << "," << millis << "\n"; 
        resultFile.flush();
        // Print the stats for every robot
        robotResultFile << id << "," << group << "," << robotTravelTimes.size() << ",";
        for(size_t i = 0; i < robotTravelTimes.size(); i++) {
            robotResultFile << robotTravelTimes[i];
            if(i != robotTravelTimes.size() - 1) {
                robotResultFile << ",";
            }
        }
        robotResultFile << "\n";
        robotResultFile.flush();
    }
    string getName() {
        return name;
    }
};

template<typename T>
vector<int> run(ResultHandler &resultHandler, T t, const WarehouseInfo& info, int nRobots, int robotCapacity, const vector<long>& seeds) {
    vector<int> results(seeds.size());
    cout << "Running: " << resultHandler.getName() << endl;
    int group = nextGroupId();
    for(size_t i = 0; i < seeds.size(); i++) {
        cout << "Completed " << (i * 100) / seeds.size() <<'%' <<  "            \r";
        cout.flush();

        clock_t begin = clock();
        Warehouse warehouse = generateRandomWarehouse(info, seeds[i]);
        auto batches = t.solve(nRobots, robotCapacity, warehouse);
        int solTime = evaluateSolutionTime(warehouse, batches, nRobots, robotCapacity);
        results[i] = solTime;
        clock_t end = clock();
        double elapsedMs = double(end - begin) * 1000.0 / CLOCKS_PER_SEC;
        auto travelTimes = getRobotTravelTimes(warehouse, batches, nRobots, robotCapacity);
        resultHandler.appendResult(group, solTime, nRobots, robotCapacity, info.packages, (int)seeds[i], (long)elapsedMs, travelTimes);
    }

    cout << "Completed 100" << '%' << endl;
    return results;
}

template<typename T>
vector<int> run(ResultHandler &resultHandler, T t, const vector<tuple<WarehouseInfo, int, int>> &runParam, vector<long>& seeds) {
    // Will run each runParam seeds.length times
    resetGroupId();
    vector<int> results;
    for(auto param : runParam) {
        auto paramResults = run(resultHandler, t, get<0>(param), get<1>(param), get<2>(param), seeds);
        results.insert(results.end(), paramResults.begin(), paramResults.end());
    }
    return results;
}

void tst(WarehouseInfo info_xxxl) {
    int nRobots = 100;
    int capcaity = 8;
    Warehouse warehouse = generateRandomWarehouse(info_xxxl, 1337);
    cw::cw c;
    auto sol = c.solve(nRobots, capcaity, warehouse);
    vector<SmallVector<PackID>> solution(sol.size());
    for(size_t i = 0; i < sol.size(); i++) {
        for(size_t j = 0; j < sol[i].size(); j++) {
            solution[i].push_back(sol[i][j]);
        }
    }

    int N = 10000;
    vector<long> clocks(N);
    cout << "Test small vectors" << endl;
    for(int i = 0; i < N; i++) {
        cout << i << "                 \r";
        cout.flush();
        tabu::BISwapperSmall bi(10, 8, nRobots);        
        clock_t c_start = clock();
        auto p = bi.doBestMove(0, warehouse, solution);
        clock_t c_end = clock();
        clocks.push_back(c_end - c_start);
        vector<vector<PackID>> sol(p.second.size());
        for(size_t y = 0; y < p.second.size(); y++) {
            for(size_t x = 0; x < p.second[y].size(); x++) {
                sol[y].push_back(p.second[y][x]);
            }
        }
        
        evaluateSolutionTime(warehouse, sol, nRobots, capcaity);
    }

    sort(clocks.begin(), clocks.end());
    clock_t p95 = clocks[clocks.size() * 0.95];
    clock_t p99 = clocks[clocks.size() * 0.99];
    clock_t median = clocks[clocks.size() / 2];
    cout << "CLOCK P95 " << p95 << ", P99: " << p99 << ", median: " << median << endl;
}

void tst2(WarehouseInfo info_xxxl) {
    int nRobots = 100;
    int capcaity = 8;
    Warehouse warehouse = generateRandomWarehouse(info_xxxl, 1337);
    cw::cw c;
    auto sol = c.solve(nRobots, capcaity, warehouse);

    int N = 10000;
    vector<long> clocks(N);
    cout << "Test normal vectors" << endl;
    for(int i = 0; i < N; i++) {
        cout << i << "                 \r";
        cout.flush();
        tabu::BISwapper bi(10);        
        clock_t c_start = clock();
        bi.doBestMove(0, warehouse, sol);
        clock_t c_end = clock();
        clocks.push_back(c_end - c_start);
    }

    sort(clocks.begin(), clocks.end());
    clock_t p95 = clocks[clocks.size() * 0.95];
    clock_t p99 = clocks[clocks.size() * 0.99];
    clock_t median = clocks[clocks.size() / 2];
    cout << "CLOCK P95 " << p95 << ", P99: " << p99 << ", median: " << median << endl;
}


void tstgeneration(WarehouseInfo info, vector<long> seeds) {
    const int N = 100;
    int NROBOTS = 5;
    int CAPACITY = 5;
    for(long seed : seeds) {
        cw::cw c;
        Warehouse old = generateRandomWarehouse(info, seed);
        auto sol = c.solve(NROBOTS, CAPACITY, old);
        for(int i = 0; i < N; i++) {
            Warehouse war = generateRandomWarehouse(info, seed);
            auto newsol = c.solve(NROBOTS,CAPACITY,old);
            for(int i = 0; i < war.getPathLengths().size(); i++) {
                for(int j = 0; j < war.getPathLengths()[0].size(); j++) {
                    if(war.getPathLengths()[i][j] != old.getPathLengths()[i][j]) {
                        cout << "P lens" << endl;
                        cout << war.to_string() << "\n\n is not the same as: \n" << old.to_string() << "\n\n iters: " << i << endl;
                        return;
                    }
                }
            }
            if(sol.size() != newsol.size()) {
                cout << "SOL SIZE" << endl;
                cout << war.to_string() << "\n\n is not the same as: \n" << old.to_string() << "\n\n iters: " << i << endl;
                return;
            }
            for(int i = 0; i < sol.size(); i++) {
                if(sol[i].size() != newsol[i].size()) {
                    cout << "SOL INNER SIZE" << endl;
                    cout << war.to_string() << "\n\n is not the same as: \n" << old.to_string() << "\n\n iters: " << i << endl;
                    return;
                }
                for(int j = 0; j < sol[i].size(); j++) {
                    if(sol[i][j] != newsol[i][j]) {
                        cout << "SOL IDX" << endl;
                        cout << war.to_string() << "\n\n is not the same as: \n" << old.to_string() << "\n\n iters: " << i << endl;
                        return;
                    }
                }
            }
            int orgt = evaluateSolutionTime(old, sol, NROBOTS, CAPACITY);
            int newt = evaluateSolutionTime(war, newsol, NROBOTS, CAPACITY);
            if(orgt != newt) {
                cout << "TIME" << endl;
                cout << war.to_string() << "\n\n is not the same as: \n" << old.to_string() << "\n\n iters: " << i << endl;
                return;

            }
        }
    }
    cout << "All are the same " << endl;
}

int main() {
    WarehouseInfo info_xs;
    info_xs.aisles = 2;
    info_xs.aisleWidth = 2;
    info_xs.crossAiles = 2;
    info_xs.crossAilesWidth = 2;
    info_xs.shelfHeight = 10;
    info_xs.packages = 10;

    WarehouseInfo info_s;
    info_s.aisles = 4;
    info_s.aisleWidth = 2;
    info_s.crossAiles = 2;
    info_s.crossAilesWidth = 2;
    info_s.shelfHeight = 20;
    info_s.packages = 40;

    WarehouseInfo info_m;
    info_m.aisles = 8;
    info_m.aisleWidth = 2;
    info_m.crossAiles = 2;
    info_m.crossAilesWidth = 2;
    info_m.shelfHeight = 20;
    info_m.packages = 80;

    WarehouseInfo info_l;
    info_l.aisles = 16;
    info_l.aisleWidth = 3;
    info_l.crossAiles = 3;
    info_l.crossAilesWidth = 3;
    info_l.shelfHeight = 20;
    info_l.packages = 160;


    WarehouseInfo info_xl;
    info_xl.aisles = 32;
    info_xl.aisleWidth = 3;
    info_xl.crossAiles = 3;
    info_xl.crossAilesWidth = 3;
    info_xl.shelfHeight = 20;
    info_xl.packages = 320;

    WarehouseInfo info_xxl;
    info_xxl.aisles = 64;
    info_xxl.aisleWidth = 3;
    info_xxl.crossAiles = 3;
    info_xxl.crossAilesWidth = 3;
    info_xxl.shelfHeight = 20;
    info_xxl.packages = 640;

    WarehouseInfo info_xxxl;
    info_xxxl.aisles = 80;
    info_xxxl.aisleWidth = 3;
    info_xxxl.crossAiles = 4;
    info_xxxl.crossAilesWidth = 3;
    info_xxxl.shelfHeight = 20;
    info_xxxl.packages = 1024;


//    tst(info_xxxl);
//    tst2(info_xxxl);


    // WarehouseInfo, nRobots, robotCapacity
    vector<tuple<WarehouseInfo, int, int>> params{
        {info_xs, 2, 5},
        {info_s, 8, 5},
        {info_m, 16, 5},
        {info_l, 32, 5},
        {info_xl, 64, 5},
        {info_xxl, 128, 5},
        {info_xxxl, 205, 5},
    };

    vector<tuple<WarehouseInfo, int, int>> params_mini{
      {info_xs, 2, 5},
    };

    // TODO Run with many more generations and bigger population size
    //tabu::StopCondition stop(80000, 250);
    tabu::StopCondition stop(10000, 60);
    auto GaBal = ga::Ga(500, 1000, 1.0, 1.0);
    auto GaEvo = ga::Ga(250, 1500, 1.0, 1.0);
    auto GaPop = ga::Ga(1000, 500, 1.0, 1.0);
    auto T = tabu::Tabu(stop);
    auto OT = tabu::OldTabu(stop);

    ResultHandler complsearch("results", "complsearch");
    ResultHandler cwsr("results", "cws");
    ResultHandler greedyr("results", "greedy");
    ResultHandler gabalr("results", "ga-bal");
    ResultHandler gaevor("results", "ga-evo");
    ResultHandler gapopr("results", "ga-pop");
    ResultHandler tabur("results", "tabu");
    ResultHandler tabuoldr("results", "old-tabu");

    auto seeds = generateSeeds(20);

//    tstgeneration(info_l, seeds);
//    return 0;
    auto cws = run(cwsr, cw::cw(), params, seeds);
    auto cmps = run(complsearch, complsearch::complsearch(), params_mini, seeds);
    auto greedys = run(greedyr, greedy::greedy(), params, seeds);
    auto tabu = run(tabur, T, params, seeds);
    auto tabuold = run(tabuoldr, OT, params, seeds);
    auto gabal = run(gabalr, GaBal, params, seeds);
    auto gaevo = run(gaevor, GaEvo, params, seeds);
    auto gapop = run(gapopr, GaPop, params, seeds);

/*
    int accCWS = 0;
    int accGreedys = 0;
    for(int i = 0; i < seeds.size(); i++) {
        accCWS += cws[i];
        accGreedys += greedys[i];
    }

    cout << "ACC cws: " << accCWS << ", ACC greedy: " << accGreedys << " greedy-cws/run: " << ((double)(accGreedys-accCWS))/(double)(seeds.size()) << endl;
*/    /*
    Warehouse warehouse = generateRandomWarehouse(info);
    std::cout << warehouse.to_string() << endl;
    auto ret = runCW(warehouse, 2, 5);
    int savings = ret.second - ret.first;
    cout << "CW took: " << ret.first << ", naive took: " << ret.second << ", savings: " << savings << endl;*/
}
