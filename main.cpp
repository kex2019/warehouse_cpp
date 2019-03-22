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

int main() {
    WarehouseInfo info_xs;
    info_xs.aisles = 2;
    info_xs.aisleWidth = 2;
    info_xs.crossAiles = 2;
    info_xs.crossAilesWidth = 2;
    info_xs.shelfHeight = 10;
    info_xs.packages = 20;

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
    info_xl.aisleWidth = 2;
    info_xl.crossAiles = 2;
    info_xl.crossAilesWidth = 2;
    info_xl.shelfHeight = 20;
    info_xl.packages = 320;

    WarehouseInfo info_xxl;
    info_xxl.aisles = 64;
    info_xxl.aisleWidth = 2;
    info_xxl.crossAiles = 2;
    info_xxl.crossAilesWidth = 2;
    info_xxl.shelfHeight = 20;
    info_xxl.packages = 640;

    // WarehouseInfo, nRobots, robotCapacity
    vector<tuple<WarehouseInfo, int, int>> params{
        {info_xs, 4, 5},
        {info_s, 8, 5},
        {info_m, 16, 5},
        {info_l, 32, 5},
        {info_xl, 64, 5},
        {info_xxl, 128, 6},
    };

    // TODO Run with many more generations and bigger population size
    auto G = ga::Ga(300, 1000, 1.0, 0.5);
    auto T = tabu::Tabu();
    ResultHandler cwsr("results", "cws");
    ResultHandler greedyr("results", "greedy");
    ResultHandler gar("results", "ga");
    ResultHandler tabur("results", "tabu");

    auto seeds = generateSeeds(20);
    
    auto cws = run(cwsr, cw::cw(), params, seeds);
    auto greedys = run(greedyr, greedy::greedy(), params, seeds);
    auto tabu = run(tabur, T, params, seeds);
    auto ga = run(gar, G, params, seeds);
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
