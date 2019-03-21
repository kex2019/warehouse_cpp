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
        int currentFirst = 0;
        int currentSecond = 0;
        int currentFirstOrder = 0;
        int currentSecondOrder = 0;
        int robotCapacity = 0;
        bool isEof = false;
        TabuList<tuple<int,int,int,int>> tabus;
        tuple<int,int,int,int> bestMove;

    public:
        vector<vector<int>> solution;
        nswapgenerator(int lifeTime, int robotCapacity, const vector<vector<int>> &solution) : solution(solution), tabus(lifeTime){}
        vector<vector<int>> next();
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
        int currentFirst = 0;
        int currentSecond = 0;
        int currentOrder = 0;
        bool isEof = false;
        int robotCapacity = 0;
        TabuList<tuple<int,int,int>> tabus;
        tuple<int,int,int> bestMove;
    public:
        vector<vector<int>> solution;
        nshiftgenerator(int lifeTime, int robotCapacity, const vector<vector<int>> &solution) : solution(solution), tabus(lifeTime){}
        vector<vector<int>> next();
        void doBestMove(int t);
        bool eof();
        tuple<int,int,int> getBestMove();
        tuple<int,int,int> getMove();
        void step(int t) {tabus.step(t);}
            void reset(const vector<vector<int>>& solution) {
            this->solution = solution;
            currentFirst = 0;
            currentSecond = 0;
            currentOrder = 0;
            isEof = false;
        }
        void setBestMove() {
            bestMove = {currentFirst, currentSecond, currentOrder};
        }

//        vector<tuple<int,int,int>> getMove();
    };

    struct Tabu {
        vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);
    };
}
