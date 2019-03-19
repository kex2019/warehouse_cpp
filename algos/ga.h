#pragma once

#include <vector>
#include "../warehouse.h"

typedef vector<vector<int>> Chromosomes;

using namespace std;
namespace ga {


  class Ga {
    public:
      int population;
      double alpha;
      double beta;
    // This vector of vector of ints is a splitted chromosome
    vector<vector<int>> solve(int nRobots, int robotCapacity, const Warehouse &warehouse);

    double fitness(Chromosomes &chromosomes, vector<double> &fitnesses, const Warehouse &warehouse, int nRobots, int robotCapacity);
    vector<int> select(Chromosomes &chromosomes, vector<double> &fitnesses, double totalFitness, double keepN);
    void crossover(Chromosomes &chromosomes, vector<int> &elitists);
    void mutate(Chromosomes &chromosomes, vector<int> &elitists);

    Ga(int population, double alpha, double beta): population(population), alpha(alpha), beta(beta) {};
  };
}



