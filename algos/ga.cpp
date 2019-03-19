#include "ga.h"
#include "../warehouse.h"


int calcSwappingDistance(vector<int> c1, vector<int> c2) {
  return 1;
}

vector<vector<int>> ga::Ga::solve(int nRobots, int robotCapacity, const Warehouse &warehouse) {
  return {};
}

double ga::Ga::fitness(Chromosomes &chromosomes, vector<double> &fitnesses, const Warehouse &warehouse, int nRobots, int robotCapacity) {
  double totalFitness = 0.0;
  for (int i = 0; i < chromosomes.size(); i++) {
    int performance = evaluateSolutionTime(warehouse, chromosomes[i], nRobots, robotCapacity);
    int swappingDistance = 0;
    for (int j = 0; j < chromosomes.size(); j++) {
      swappingDistance += calcSwappingDistance(chromosomes[i], chromosomes[j]);
    }
    // TODO: Normalize swapping and performance scores?
    fitnesses[i] = this->alpha * double(performance) + this->beta * double(swappingDistance);
    totalFitness += fitnesses[i];
  }
  return totalFitness;
}

vector<int> ga::Ga::select(Chromosomes &chromosomes, vector<double> &fitnesses, double totalFitness, double keepN) {
  // Nice pseudo code here https://en.wikipedia.org/wiki/Stochastic_universal_sampling
  // Paper referenced in Kex Roulette-wheel selection via stochastic acceptance

  int p = int(totalFitness / keepN);
  return {};
}

void ga::Ga::crossover(Chromosomes &chromosomes, vector<int> &elitists) {
  return;
}

void ga::Ga::mutate(Chromosomes &chromosomes, vector<int> &elitists) {
  return;
}
