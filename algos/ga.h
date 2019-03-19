#pragma once

#include <vector>
#include "../warehouse.h"
#include <set>

typedef vector<vector<int>> Chromosomes;

using namespace std;
namespace ga {


  class Ga {
    set<int> orders;
    vector<int> numObsOrders;
    int chromosomeSize;
    public:
    int population;
    int generations;
    double alpha;
    double beta;
    // This vector of vector of ints is a splitted chromosome
    vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);

    double fitness(Chromosomes &chromosomes, vector<double> &fitnesses, int nRobots, int robotCapacity, const Warehouse &warehouse);
    vector<int> select(vector<double> &fitnesses, double totalFitness, double keepN);
    void crossovermutate(Chromosomes &chromosomes, vector<int> &elitists, int mutateN);

    Ga(int population, 
        int generations, 
        double alpha, 
        double beta): population(population), 
    generations(generations), 
    alpha(alpha), 
    beta(beta) {};
  };
}



