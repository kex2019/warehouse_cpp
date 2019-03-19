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
    class nswapgenerator {
        int currentFirst = 0;
        int currentSecond = 0;
        int currentFirstOrder = 0;
        int currentSecondOrder = 0;
        bool isEof = false;
    public:
        vector<vector<int>> solution;
        nswapgenerator(const vector<vector<int>> &solution) : solution(solution){}
        vector<vector<int>> next();
        bool eof();
        tuple<int,int,int,int> getMove();
    };
    class nshiftgenerator {
        int currentFirst = 0;
        int currentSecond = 0;
        int currentOrder = 0;
        bool isEof = false;
    public:
        vector<vector<int>> solution;
        nshiftgenerator(const vector<vector<int>> &solution) : solution(solution){}
        vector<vector<int>> next();
        bool eof();
//        vector<tuple<int,int,int>> getMove();
    };

    struct Tabu {
        vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);
    };

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
}
