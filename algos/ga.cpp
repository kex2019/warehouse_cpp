#include "ga.h"
#include "../warehouse.h"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "greedy.h"

// Or maybe: Adjacent Swaps on Strings
// If we need linear, use Cayley distance? (eg. cycles in the permutation). compose c1 with the inverse of c2 and count the number of cycles
int calcSwappingDistance(vector<PackID> c1, vector<PackID> c2) {
  int nSame = 0;
  for(size_t i = 0; i < c1.size(); i++) {
    if(c1[i] == c2[i])
      nSame++;
  }

  return c1.size() - nSame;
}

vector<vector<PackID>> ga::Ga::solve(size_t nRobots, 
    size_t robotCapacity, 
    const Warehouse &warehouse, function<void(long,long)> tscb) {

  // TODO: Read this paper Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization
  // To implement adaptive values for keepN and mutateN


  //  cout << "\nStarting GA -- " << "Generations: " << this->generations << " -- Population: " << this->population << "\n";
  // Generate chromosomes
  size_t orders = warehouse.getPackageLocations().size();
  vector<PackID> baseChromosome;

  //// Add orders to chromosome
  //for (size_t i = 0; i < orders; i++)
    //baseChromosome.push_back(static_cast<int>(i));

  greedy::greedy greedSolver = greedy::greedy{};
  vector<vector<PackID>> greedySolution = greedSolver.solve(nRobots, robotCapacity, warehouse, tscb);
  long _tiq = evaluateSolutionTime(warehouse, greedySolution, nRobots, robotCapacity);
  tscb(0, _tiq);
  for (auto V: greedySolution) {
    for (auto v: V) {
      baseChromosome.push_back(v);
    }

    for (size_t i = 0; i < robotCapacity - V.size(); i++) {
      baseChromosome.push_back(-1);
    }
  }

  if (robotCapacity * nRobots < orders)
    throw runtime_error("Cannot plan when more orders than can be picked up.");

  // Pad the chromosomes
  //for (size_t i = 0; i < robotCapacity * nRobots - orders; i++)
  //  baseChromosome.push_back(-1);

  this->chromosomeSize = robotCapacity * nRobots;
  this->numObsOrders = vector<int>(orders);
  this->apexChromosome = vector<PackID>{};
  this->apexPerformance = -1e5;
  this->chromosomeIDs.clear();

  for (size_t i = 0; i < this->population; i++)
    this->chromosomeIDs.push_back(static_cast<int>(i));


  if (baseChromosome.size() != this->chromosomeSize) {
    cout << robotCapacity * nRobots << endl;
    cout << orders << endl;
    cout << baseChromosome.size() << " != " << this->chromosomeSize << endl;
    throw runtime_error("baseChromosome size does not match chromosome size");
  }

  Chromosomes chromosomes;
  for (size_t i = 0; i < this->population; i++) {
    //shuffle(baseChromosome.begin(), baseChromosome.end(), default_random_engine(this->rng()));
    chromosomes.push_back(baseChromosome);
  }

  vector<double> fitnesses(chromosomes.size());
  this->performances = vector<double>(chromosomes.size());
  this->differences = vector<double>(chromosomes.size());


  long oldApexPerf = apexPerformance;
  // Fit f 
  // f(0) = max_exploration
  // f(generations) = min_exploration
  // aB^x
  //
  // Example
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

  double selectStart = 0.1;
  double selectEnd = 3.0 / this->population;

  double selectB = exp(log(selectEnd * 2) / (this->generations + 1e-6));
  double selectA = selectStart;

  clock_t tottime = clock();
  //double selectA
  for (size_t g = 0; g < this->generations; g++) {

    this->fitness(chromosomes, fitnesses, nRobots, robotCapacity, warehouse);
    
    int keepN = max(3, int(selectA*pow(selectB, g) * this->population));

    vector<int> elitists = this->select(fitnesses, keepN);

    this->crossover(chromosomes, elitists);

    //cout << this->apexPerformance << " " << "Keep " << keepN << " " << this->population <<"\n";
    this->mutate(chromosomes, fitnesses);

    if(oldApexPerf < apexPerformance) {
      clock_t lastTime = clock();
      tscb((lastTime - tottime) / ((double)CLOCKS_PER_SEC / 1000), -apexPerformance);
      oldApexPerf = apexPerformance;
    }
  }

  vector<vector<PackID>> solution;
  vector<PackID> batch;
  for (size_t i = 0; i < chromosomeSize; i++) {
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

  clock_t lastTime = clock();
  tscb((lastTime - tottime) / ((double)CLOCKS_PER_SEC / 1000), -apexPerformance);

  return solution;
}

void ga::Ga::fitness(Chromosomes &chromosomes, 
    vector<double> &fitnesses, 
    int nRobots, 
    int robotCapacity, 
    const Warehouse &warehouse) {

  double performanceMean = 0.0;
  double differenceMean = 0.0;
  for (size_t i = 0; i < chromosomes.size(); i++) {
    double performance = double(-evaluateSolutionTime(warehouse, chromosomes[i], nRobots, robotCapacity));

    // Update apex
    if (performance > this->apexPerformance) {
      this->apexPerformance = performance;
      this->apexChromosome = chromosomes[i];
    }

    int swappingDistance = 0;
    for (size_t j = 0; j < chromosomes.size(); j++) {
      swappingDistance += calcSwappingDistance(chromosomes[i], chromosomes[j]);
    }

    this->performances[i] = performance;
    this->differences[i] = swappingDistance;

    performanceMean += performance;
    differenceMean += swappingDistance;
  }

  // Ranking
  //sort(this->chromosomeIDs.begin(), this->chromosomeIDs.end(), [this](int i, int j) {
      //return this->performances[i] < this->performances[j];
      //});

  //for (int i = 0; i < this->chromosomeIDs.size(); i++) {
    //fitnesses[this->chromosomeIDs[i]] = this->alpha * i;
  //}

  //sort(this->chromosomeIDs.begin(), this->chromosomeIDs.end(), [this](int i, int j) {
      //return this->differences[i] < this->differences[j];
      //});

  //for (int i = 0; i < this->chromosomeIDs.size(); i++) {
    //fitnesses[this->chromosomeIDs[i]] += this->beta * i;
  //}

  performanceMean /= double(chromosomes.size());
  differenceMean /= double(chromosomes.size());

  double performanceStd = 0.0;
  double differenceStd = 0.0;
  //Calculate variances
  for (size_t i = 0; i < chromosomes.size(); i++) {
    performanceStd += (performances[i] - performanceMean) * (performances[i] - performanceMean);
    differenceStd += (differences[i] - differenceMean) * (differences[i] - differenceMean);
  }

  // Calculate sample standard deviation
  performanceStd /= sqrt(chromosomes.size() - 1);
  differenceStd /= sqrt(chromosomes.size() - 1);

  // Now weigh the normalized scores
  for (size_t i = 0; i < chromosomes.size(); i++) {
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
  size_t elitistIndex = 0;
  for (size_t i = 0; i < chromosomes.size(); i++) {
    if (elitistIndex < elitists.size() && static_cast<int>(i) == elitists[elitistIndex]) {
      elitistIndex++;
    } else {
      // Select two elitists & combine them and replace the chromosome at position i with that chromosome
      int e1 = elitists[this->rng() % elitists.size()];
      int e2 = elitists[this->rng() % elitists.size()];


      // Partition indexes
      int p1 = min((int)(this->rng() % this->chromosomeSize) + 5, (int)(chromosomeSize - 1));
      int p2 = this->rng() % (p1 - 1);

      // Combine the elitists into the new chromosome
      for (int j = p2; j < p1; j++)
        chromosomes[i][j] = chromosomes[e1][j];

      for (int j = 0; j < p2; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      for (size_t j = p1; j < this->chromosomeSize; j++)
        chromosomes[i][j] = chromosomes[e2][j];

      // Empty data (For chromosome validation)
      fill(this->numObsOrders.begin(), this->numObsOrders.end(), 0);
      // vector with num orders
      for (size_t j = 0; j < this->chromosomeSize; j++) {
        if (chromosomes[i][j] != -1) {
          this->numObsOrders[chromosomes[i][j]]++;
        }
      }

      // Find missing orders
      vector<int> missingOrders;
      for(size_t j = 0; j < this->numObsOrders.size(); j++) {
        if (this->numObsOrders[j] == 0) {
          missingOrders.push_back(j);
        }
      }

      // Adjust for missing orders by removing duplicates
      for (size_t j = 0; j < this->chromosomeSize && missingOrders.size(); j++) {
        if (chromosomes[i][j] != -1 && this->numObsOrders[chromosomes[i][j]] == 2) {
          this->numObsOrders[chromosomes[i][j]]--;
          chromosomes[i][j] = missingOrders.front();
          missingOrders.erase(missingOrders.begin());
        }
      }

      // Adjust for missing orders by filling padding
      for (size_t j = 0; j < this->chromosomeSize && missingOrders.size(); j++) {
        if (chromosomes[i][j] == -1) {
          chromosomes[i][j] = missingOrders.front();
          missingOrders.erase(missingOrders.begin());
        }
      }


      // Remove all duplicates
      for (size_t j = 0; j < this->chromosomeSize; j++) {
        if (this->numObsOrders[chromosomes[i][j]] == 2) {
          numObsOrders[chromosomes[i][j]]--;
          chromosomes[i][j] = -1;
        }
      }
    }
  }
}

void ga::Ga::mutate(Chromosomes &chromosomes, vector<double> &fitnesses) {

  for (size_t i = 0; i < chromosomes.size(); i++) {
    int mutate = 1; //int(a*pow(b, fitnesses[i] + offset) * mutations);
    for (int j = 0; j < mutate; j++) {
      int i1 = this->rng() % this->chromosomeSize;
      int i2 = this->rng() % this->chromosomeSize;
      swap(chromosomes[i][i1], chromosomes[i][i2]);
    }
  }
}
