#pragma once

#include <vector>
#include "../warehouse.h"
#include <set>
#include <random>
#include <time.h>

typedef vector<vector<int>> Chromosomes;

using namespace std;
namespace ga {

  class Ga {
    vector<int> numObsOrders;
    unsigned int chromosomeSize;
    vector<int> chromosomeIDs;
    vector<int> apexChromosome;
    double apexPerformance;

    vector<double> performances;
    vector<double> differences;

    mt19937 rng;
    public:
    size_t population;
    size_t generations;
    double alpha;
    double beta;
    // This vector of vector of ints is a splitted chromosome
    vector<vector<int>> solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse);

    void fitness(Chromosomes &chromosomes, vector<double> &fitnesses, int nRobots, int robotCapacity, const Warehouse &warehouse);
    vector<int> select(vector<double> &fitnesses, double keepN);
    void crossover(Chromosomes &chromosomes, vector<int> &elitists);
    void mutate(Chromosomes &chromosomes, vector<double> &fitnesses, int mutations);

    Ga(size_t population, 
        size_t generations, 
        double alpha, 
        double beta): rng(10), population(population), 
    generations(generations), 
    alpha(alpha), 
    beta(beta) {};
  };
}



