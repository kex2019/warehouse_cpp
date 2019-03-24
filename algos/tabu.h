#pragma once
#include <vector>
#include "../warehouse.h"
#include "cw.h"
#include <utility>
#include <set>
#include <tuple>
using namespace std;
// C&W algorithm for creating batches
namespace tabu {
    template<typename T>
    // List of tabus
    class TabuList {
        vector<pair<int,T>> tabus;
        set<T> tabusOrdered;
        int lifeTime;
    public:
        TabuList(int lifeTime) : lifeTime(lifeTime) {}
        void step(int t) {
            while(tabus.size() && tabus.front().first <= t) {
                tabusOrdered.erase(tabus.front().second);
                tabus.erase(tabus.begin());
            }
        }
        // Assumes that t[i + 1] >= t[i]
        void insertTabu(int t, T tabu) {
            tabus.push_back({lifeTime + t, tabu});
            tabusOrdered.insert(tabu);
        }

        bool isTabu(T t) {
            return tabusOrdered.find(t) != tabusOrdered.end();
        }
    };

    class nswapgenerator {
        size_t currentFirst = 0;
        size_t currentSecond = 0;
        size_t currentFirstOrder = 0;
        size_t currentSecondOrder = 0;
        size_t robotCapacity = 0;
        size_t nRobots = 0;
        bool isEof = false;
        TabuList<tuple<int,int,int,int>> tabus;
        tuple<int,int,int,int> bestMove;
        set<tuple<int,int,int,int>> checked;
    public:
        vector<vector<int>> solution;
        nswapgenerator(int lifeTime, size_t nRobots, size_t robotCapacity, const vector<vector<int>> &solution) : robotCapacity(robotCapacity), nRobots(nRobots), tabus(lifeTime), solution(solution){}
        pair<vector<vector<int>>, bool> next();
        bool eof();
        tuple<int,int,int,int> getBestMove();
        tuple<int,int,int,int> getMove();
        void doBestMove(int t);
        void step(int t) {tabus.step(t);}
        void reset(const vector<vector<int>>& solution) {
            this->solution = solution;
            currentFirstOrder = 0;
            currentFirst = 0;
            currentSecond = 0;
            currentSecondOrder = 0;
            isEof = false;
        }
        void setBestMove() {
            bestMove = {currentFirst, currentSecond, currentFirstOrder, currentSecondOrder};
        }
    };
    class nshiftgenerator {
        size_t currentFirst = 0;
        size_t currentSecond = 0;
        int currentOrder = -1;
        size_t nRobots = 0;
        bool isEof = false;
        size_t robotCapacity = 0;
        TabuList<tuple<int,int,int>> tabus;
        tuple<int,int,int> bestMove;
        set<tuple<int,int,int>> checked;
    public:
        vector<vector<int>> solution;
        nshiftgenerator(int lifeTime, size_t nRobots, size_t robotCapacity, const vector<vector<int>> &solution) : nRobots(nRobots), robotCapacity(robotCapacity), tabus(lifeTime), solution(solution){}
        pair<vector<vector<int>>, bool> next();
        void doBestMove(int t);
        bool eof();
        tuple<int,int,int> getBestMove();
        tuple<int,int,int> getMove();
        void step(int t) {tabus.step(t);}
        void reset(const vector<vector<int>>& solution) {
            this->solution = solution;
            currentFirst = 0;
            currentSecond = 0;
            currentOrder = -1;
            isEof = false;
            checked.clear();
        }
        void setBestMove() {
            bestMove = {currentFirst, currentSecond, currentOrder};
        }
    };
    class ncomgenerator {
        size_t current = 0;
        size_t nRobots = 0;
        size_t robotCapacity = 0;
        bool isEof = false;
        nshiftgenerator nshift;
        nswapgenerator nswap;
    public:
        ncomgenerator(int lifeTime, size_t nRobots, size_t robotCapacity, const vector<vector<int>> &solution) : nRobots(nRobots), robotCapacity(robotCapacity), nshift(lifeTime, nRobots, robotCapacity, solution), nswap(lifeTime, nRobots, robotCapacity, solution) {}
//        pair<vector<vector<int>>, bool> next();
//        void doBestMove(int t);
//        bool eof();
//        void step(int t);
//        void reset(const vector<vector<int>>& solution);
    };

    struct Tabu {
        vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);
    };
}
