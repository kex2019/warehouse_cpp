#pragma once

#include <vector>
#include "../warehouse.h"


using namespace std;
namespace ga {

  typedef vector<vector<int>> Chromosomes;

  class Ga {
    int population;
    // This vector of vector of ints is a splitted chromosome
    vector<vector<int>> solve(int robotCapacity, const Warehouse &warehouse);

    void fitness(Chromosomes &chromosomes, vector<int> &fitnesses);
    vector<int> select(Chromosomes &chromosomes, vector<int> &fitnesses);
    void crossover(Chromosomes &chromosomes, vector<int> elitists);
    void mutate(Chromosomes &chromosomes, vector<int> elitists);
  };
}



