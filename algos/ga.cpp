#include "ga.h"
#include "../warehouse.h"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm>

// Or maybe: Adjacent Swaps on Strings
// If we need linear, use Cayley distance? (eg. cycles in the permutation). compose c1 with the inverse of c2 and count the number of cycles
int calcSwappingDistance(vector<int> c1, vector<int> c2) {
  int nSame = 0;
  for(int i = 0; i < c1.size(); i++) {
    if(c1[i] == c2[i])
      nSame++;
  }

  return c1.size() - nSame;
}

vector<vector<int>> ga::Ga::solve(int nRobots, 
    int robotCapacity, 
    const Warehouse &warehouse) {

  // TODO: Read this paper Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization
  // To implement adaptive values for keepN and mutateN


  //  cout << "\nStarting GA -- " << "Generations: " << this->generations << " -- Population: " << this->population << "\n";
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
  this->apexChromosome = vector<int>{};
  this->apexPerformance = -1e5;
  this->chromosomeIDs.clear();

  for (int i = 0; i < this->population; i++)
    this->chromosomeIDs.push_back(i);


  if (baseChromosome.size() != this->chromosomeSize)
    throw runtime_error("baseChromosome size does not match chromosome size");

  Chromosomes chromosomes;
  for (int i = 0; i < this->population; i++) {
    shuffle(baseChromosome.begin(), baseChromosome.end(), default_random_engine(this->rng()));
    chromosomes.push_back(baseChromosome);
  }

  double totalFitness = 0;
  vector<double> fitnesses(chromosomes.size());
  this->performances = vector<double>(chromosomes.size());
  this->differences = vector<double>(chromosomes.size());

  for (int g = 0; g < this->generations; g++) {
    this->fitness(chromosomes, fitnesses, nRobots, robotCapacity, warehouse);


    // TODO: Calculate how many to keep (Currently naive)
    int keepN = int(this->population * 0.2);

    vector<int> elitists = this->select(fitnesses, keepN);

    // TODO: Calculate how much to mutate (Currenly naive)
    this->crossover(chromosomes, elitists);

    int mutateN = int(this->chromosomeSize);
    this->mutate(chromosomes, fitnesses, mutateN);
  }


  vector<vector<int>> solution;
  vector<int> batch;
  for (int i = 0; i < chromosomeSize; i++) {
    if (i % robotCapacity == 0 && i != 0) {
      solution.push_back(batch);
      batch.clear();
    }
    if (this->apexChromosome[i] != -1) {
      batch.push_back(this->apexChromosome[i]);
    }
  }

  if (solution.size() != nRobots)
    solution.push_back(batch);

  return solution;
}

void ga::Ga::fitness(Chromosomes &chromosomes, 
    vector<double> &fitnesses, 
    int nRobots, 
    int robotCapacity, 
    const Warehouse &warehouse) {

  double performanceMean = 0.0;
  double differenceMean = 0.0;
  for (int i = 0; i < chromosomes.size(); i++) {
    double performance = double(-evaluateSolutionTime(warehouse, chromosomes[i], nRobots, robotCapacity));

    // Update apex
    if (performance > this->apexPerformance) {
      this->apexPerformance = performance;
      this->apexChromosome = chromosomes[i];
    }

    int swappingDistance = 0;
    for (int j = 0; j < chromosomes.size(); j++) {
      swappingDistance += calcSwappingDistance(chromosomes[i], chromosomes[j]);
    }

    this->performances[i] = performance;
    this->differences[i] = swappingDistance;

    performanceMean += performance;
    differenceMean += swappingDistance;
  }

  performanceMean /= double(chromosomes.size());
  differenceMean /= double(chromosomes.size());

  double performanceStd = 0.0;
  double differenceStd = 0.0;
  //Calculate variances
  for (int i = 0; i < chromosomes.size(); i++) {
    performanceStd += (performances[i] - performanceMean) * (performances[i] - performanceMean);
    differenceStd += (differences[i] - differenceMean) * (differences[i] - differenceMean);
  }

  // Calculate sample standard deviation
  performanceStd /= sqrt(chromosomes.size() - 1);
  differenceStd /= sqrt(chromosomes.size() - 1);

  // Now weigh the normalized scores
  for (int i = 0; i < chromosomes.size(); i++) {
    fitnesses[i] = this->alpha * ((performances[i] - performanceMean) / (performanceStd + 1e-4)) 
      + this->beta * ((differences[i] - differenceMean) / (differenceStd + 1e-4));
  }


  return;
}

vector<int> ga::Ga::select(vector<double> &fitnesses, 
    double keepN) {

  sort(this->chromosomeIDs.begin(), this->chromosomeIDs.end(), [fitnesses](int i, int j) {
      return fitnesses[i] > fitnesses[j];
      });

  vector<int> elitists;
  for (int i = 0; i < keepN; i++)
    elitists.push_back(this->chromosomeIDs[i]);

  return elitists;
}

void ga::Ga::crossover(Chromosomes &chromosomes, 
    vector<int> &elitists) {
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
    }
  }
}

void ga::Ga::mutate(Chromosomes &chromosomes, vector<double> &fitnesses, int mutations) {

  double max_fit = 0.0;
  double min_fit = 0.0;
  for (int i = 0; i < fitnesses.size(); i++) {
    max_fit = max(max_fit, fitnesses[i]);
    min_fit = min(min_fit, fitnesses[i]);
  }

  // Have a offset because not sure if negative values is ok.
  double offset = abs(min(0.0, min_fit));

  min_fit += offset;
  max_fit += offset;


  // Fit f 
  // f(min) = 0.5
  // f(max) = 0.01
  // aB^x
  // 
  // ab^min = 0.5
  // ab^max = 0.01
  //
  // a = 0.5/(b^min)
  // b^max * 0.5/(b^min) = 0.01
  // b^(max - min) * 0.5 = 0.01
  // b^(max - min) = 0.02
  // log(b^(max - min)) = (max - min)log(b)
  // b = e^(log(0.02) / (max - min))
  // a = 0.5/(b^min)

  double min_mut = 0.01;
  double max_mut = 0.5;

  double b = exp(log(min_mut * 2) / (max_fit - min_fit + 1e-6));
  double a = max_mut / pow(b, min_fit);

  for (int i = 0; i < chromosomes.size(); i++) {
    int mutate = a*pow(b, fitnesses[i] + offset) * mutations;
    for (int j = 0; j < mutate; j++) {
      int i1 = this->rng() % this->chromosomeSize;
      int i2 = this->rng() % this->chromosomeSize;
      swap(chromosomes[i][i1], chromosomes[i][i2]);
    }
  }
}
