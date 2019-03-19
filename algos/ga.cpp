#include "ga.h"
#include "../warehouse.h"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm>


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
  this->numObsOrders = vector<int>(orders);


  if (baseChromosome.size() != this->chromosomeSize)
    throw runtime_error("baseChromosome size does not match chromosome size");

  Chromosomes chromosomes;
  for (int i = 0; i < this->population; i++) {
    shuffle(baseChromosome.begin(), baseChromosome.end(), default_random_engine(this->rng()));
    chromosomes.push_back(baseChromosome);
  }

  double totalFitness = 0;
  vector<double> fitnesses(chromosomes.size());

  vector<int> apexChromosome;
  double apexFitness = 0.0;

  for (int g = 0; g < this->generations; g++) {
    cout << "Generation " << g << "               \r";
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

  if (solution.size() != nRobots)
    solution.push_back(batch);

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
  int start = this->rng() % pDistance;

  vector<int> elitists;
  for (int i = 0; i < keepN; i++) {
    int p = start + i * pDistance;
    unsigned long j = 0;
    double jsum = 0.0;
    while (jsum < p && j < fitnesses.size()) {
      jsum += fitnesses[j];
      j++;
    }
    elitists.push_back(min(j, fitnesses.size() - 1));
  }

  return elitists;
}

void ga::Ga::crossovermutate(Chromosomes &chromosomes, 
    vector<int> &elitists, 
    int mutateN) {
  // Combine elitists to populate all chromosomes that is not in elitists
  int elitistIndex = 0;
  for (int i = 0; i < chromosomes.size(); i++) {
    if (elitistIndex < elitists.size() && i == elitists[elitistIndex]) {
      elitistIndex++;
    } else {
      // Select two elitists & combine them and replace the chromosome at position i with that chromosome
      int e1 = elitists[this->rng() % elitists.size()];
      int e2 = elitists[this->rng() % elitists.size()];
      // Partition indexes
      int p1 = min((int)(this->rng() % this->chromosomeSize) + 2, (int)(chromosomeSize - 1));
      int p2 = this->rng() % (p1 - 1);

      // Combine the elitists into the new chromosome
      for (int j = p2; j < p1; j++)
        chromosomes[i][j] = chromosomes[e1][j];

      for (int j = 0; j < p2; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      for (int j = p1; j < this->chromosomeSize; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      // Empty data (For chromosome validation)
      fill(this->numObsOrders.begin(), this->numObsOrders.end(), 0);
      // vector with num orders
      for (int j = 0; j < this->chromosomeSize; j++) {
        if (chromosomes[i][j] != -1) {
          this->numObsOrders[chromosomes[i][j]]++;
        }
      }

      // Find missing orders
      vector<int> missingOrders;
      for(int j = 0; j < this->numObsOrders.size(); j++) {
        if (this->numObsOrders[j] == 0) {
          missingOrders.push_back(j);
        }
      }

      // Adjust for missing orders by removing duplicates
      for (int j = 0; j < this->chromosomeSize && missingOrders.size(); j++) {
        if(numObsOrders[chromosomes[i][j]] > 2) {
          throw runtime_error("An order contained omre than 2 orders");
        }

        if (chromosomes[i][j] != -1 && this->numObsOrders[chromosomes[i][j]] == 2) {
          this->numObsOrders[chromosomes[i][j]]--;
          chromosomes[i][j] = missingOrders.front();
          missingOrders.erase(missingOrders.begin());
        }
      }

      // Adjust for missing orders by filling padding
      for (int j = 0; j < this->chromosomeSize && missingOrders.size(); j++) {
        if (chromosomes[i][j] == -1) {
          chromosomes[i][j] = missingOrders.front();
          missingOrders.erase(missingOrders.begin());
        }
      }

      // Remove all duplicates
      for (int j = 0; j < this->chromosomeSize; j++) {
        if (this->numObsOrders[chromosomes[i][j]] == 2) {
          numObsOrders[chromosomes[i][j]]--;
          chromosomes[i][j] = -1;
        }
      }

      // Now mutate the chromosome
      for (int j = 0; j < mutateN; j++) {
        int i1 = this->rng() % this->chromosomeSize;
        int i2 = this->rng() % this->chromosomeSize;
        swap(chromosomes[i][i1], chromosomes[i][i2]);
      }

    }
  }
}
