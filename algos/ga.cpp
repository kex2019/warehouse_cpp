#include "ga.h"
#include "../warehouse.h"
#include <random>
#include <cmath>
#include <iostream>


int calcSwappingDistance(vector<int> c1, vector<int> c2) {
  return 1;
}

vector<vector<int>> ga::Ga::solve(int nRobots, 
    int robotCapacity, 
    const Warehouse &warehouse) {

  // TODO: Read this paper Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization
  // To implement adaptive values for keepN and mutateN


  cout << "\nStarting GA -- " << "Generations: " << this->generations << " -- Population: " << this->population << "\n";
  // Generate chromosomes
  int orders = warehouse.getPackageLocations().size();


  vector<int> baseChromosome;

  // Add orders to chromosome
  for (int i = 0; i < orders; i++)
    baseChromosome.push_back(i);

  if (robotCapacity * nRobots < orders)
    throw runtime_error("Cannot plan when more orders than can be picked up.");

  // Pad the chromosomes
  for (int i = 0; i < robotCapacity * nRobots - orders; i++)
    baseChromosome.push_back(-1);

  this->chromosomeSize = robotCapacity * nRobots;

  if (baseChromosome.size() != this->chromosomeSize)
    throw runtime_error("baseChromosome size does not match chromosome size");

  Chromosomes chromosomes;
  for (int i = 0; i < this->population; i++) {
    shuffle(baseChromosome);
    chromosomes.push_back(baseChromosome);
  }

  double totalFitness = 0;
  vector<double> fitnesses(chromosomes.size());

  vector<int> apexChromosome;
  double apexFitness = 0.0;
  for (int g = 0; g < this->generations; g++) {
    totalFitness = this->fitness(chromosomes, fitnesses, nRobots, robotCapacity, warehouse);

    // Store apex
    for (int i = 0; i < fitnesses.size(); i++) {
      if (fitnesses[i] > apexFitness) {
        apexFitness = fitnesses[i];
        apexChromosome = chromosomes[i];
      }

    }

    // TODO: Calculate how many to keep (Currently naive)
    int keepN = int(this->population * 0.4);

    vector<int> elitists = this->select(fitnesses, totalFitness, keepN);

    // TODO: Calculate how much to mutate (Currenly naive)
    int mutateN = max(1, int(this->chromosomeSize * 0.1));

    this->crossovermutate(chromosomes, elitists, mutateN);

    cout << "Generation " << g << "               \r";
  }

  vector<vector<int>> solution;
  vector<int> batch;
  for (int i = 0; i < chromosomeSize; i++) {
    if (i % robotCapacity == 0 && i != 0) {
      solution.push_back(batch);
      batch.clear();
    }
    if (apexChromosome[i] != -1) {
      batch.push_back(apexChromosome[i]);
    }
  }

  return solution;
}

double ga::Ga::fitness(Chromosomes &chromosomes, 
    vector<double> &fitnesses, 
    int nRobots, 
    int robotCapacity, 
    const Warehouse &warehouse) {
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

vector<int> ga::Ga::select(vector<double> &fitnesses, 
    double totalFitness, 
    double keepN) {
  // Nice pseudo code here https://en.wikipedia.org/wiki/Stochastic_universal_sampling
  // Paper referenced in Kex Roulette-wheel selection via stochastic acceptance

  int pDistance = int(totalFitness / keepN);
  int start = rng() % pDistance;

  vector<int> elitists;
  for (int i = 0; i < keepN; i++) {
    int p = start + i * pDistance;
    int j = 0;
    double jsum = 0.0;
    while (jsum < p && j < fitnesses.size()) {
      jsum += fitnesses[j];
      j++;
    }
    elitists.push_back(j);
  }

  return elitists;
}

void ga::Ga::crossovermutate(Chromosomes &chromosomes, 
    vector<int> &elitists, 
    int mutateN) {
  // Combine elitists to populate all chromosomes that is not in elitists
  vector<int> defectives;
  int elitistIndex = 0;
  for (int i = 0; i < chromosomes.size(); i++) {
    if (elitistIndex < elitists.size() && i == elitists[elitistIndex]) {
      elitistIndex++;
    } else {
      // Select two elitists & combine them and replace the chromosome at position i with that chromosome
      int e1 = elitists[rng() % elitists.size()];
      int e2 = elitists[rng() % elitists.size()];

      // Partition indexes
      int p1 = rng() % this->chromosomeSize;
      int p2 = rng() % (p1 - 1);

      // Combine the elitists into the new chromosome
      for (int j = p1; j < p2; j++)
        chromosomes[i][j] = chromosomes[e1][j];

      for (int j = 0; j < p1; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      for (int j = p2; j < this->chromosomeSize; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      // Empty data (For chromosome validation)
      fill(this->numObsOrders.begin(), this->numObsOrders.end(), 0);

      // Update set with observed orders and vector with num orders
      for (int j = 0; j < this->chromosomeSize; j++) 
        this->numObsOrders[chromosomes[i][j]]++;

      // Find missing orders
      vector<int> missingOrders;
      for(int j = 0; j < this->chromosomeSize; j++) 
        if (this->numObsOrders[j] == 0) 
          missingOrders.push_back(j);

      // Adjust for missing orders
      int missingOrderIndex = 0;
      for (int j = 0; j < this->chromosomeSize && missingOrderIndex < missingOrders.size(); j++)
        if (this->numObsOrders[j] == 2) 
          chromosomes[i][j] = missingOrders[missingOrderIndex++];

      for (int j = 0; j < mutateN; j++) {
        int i1 = rng() % this->chromosomeSize;
        int i2 = rng() % this->chromosomeSize;
        swap(chromosomes[i][i1], chromosomes[i][i2]);
      }
      // Now mutate the chromosome
    }
  }
  return;
}
