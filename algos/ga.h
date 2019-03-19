#pragma once

#include <vector>
#include "../warehouse.h"

typedef vector<vector<int>> Chromosomes;

using namespace std;
namespace ga {


  class Ga {
    public:
      int population;
    // This vector of vector of ints is a splitted chromosome
    vector<vector<int>> solve(int robotCapacity, const Warehouse &warehouse);

    void fitness(Chromosomes &chromosomes, vector<int> &fitnesses, const Warehouse &warehouse, int robotCapacity);
    vector<int> select(Chromosomes &chromosomes, vector<int> &fitnesses);
    void crossover(Chromosomes &chromosomes, vector<int> &elitists);
    void mutate(Chromosomes &chromosomes, vector<int> &elitists);

    Ga(int population): population(population) {};
  };
}



