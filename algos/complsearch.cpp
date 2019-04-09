#include "complsearch.h"
#include <iostream>

bool LEFT_TO_RIGHT = true; 
bool RIGHT_TO_LEFT = false; 

// Utility functions for finding the 
// position of largest mobile integer in a[]. 
int searchArr(vector<PackID> &a, int n, int mobile) { 
  for (int i = 0; i < n; i++) 
    if (a[i] == mobile) 
      return i + 1; 
  return -1;
} 

int fact(int n) { 
  int res = 1; 
  for (int i = 1; i <= n; i++) 
    res = res * i; 
  return res; 
} 

// To carry out step 1 of the algorithm i.e. 
// to find the largest mobile integer. 
int getMobile(vector<PackID> &a, vector<bool> &dir, int n) { 
  int mobile_prev = 0, mobile = 0; 
  for (int i = 0; i < n; i++) { 
    // direction 0 represents RIGHT TO LEFT. 
    if (dir[a[i]-1] == RIGHT_TO_LEFT && i!=0) { 
      if (a[i] > a[i-1] && a[i] > mobile_prev) { 
        mobile = a[i]; 
        mobile_prev = mobile; 
      } 
    } 

    // direction 1 represents LEFT TO RIGHT. 
    if (dir[a[i]-1] == LEFT_TO_RIGHT && i!=n-1) { 
      if (a[i] > a[i+1] && a[i] > mobile_prev) { 
        mobile = a[i]; 
        mobile_prev = mobile; 
      } 
    } 
  } 

  if (mobile == 0 && mobile_prev == 0) 
    return 0; 
  else
    return mobile; 
} 

void nextSolution(vector<PackID> &a, vector<bool> &dir, int n) { 
  int mobile = getMobile(a, dir, n); 
  int pos = searchArr(a, n, mobile); 

  // swapping the elements according to the 
  // direction i.e. dir[]. 
  if (dir[a[pos - 1] - 1] ==  RIGHT_TO_LEFT) 
    swap(a[pos-1], a[pos-2]); 

  else if (dir[a[pos - 1] - 1] == LEFT_TO_RIGHT) 
    swap(a[pos], a[pos-1]); 

  // changing the directions for elements 
  // greater than largest mobile integer. 
  for (int i = 0; i < n; i++) { 
    if (a[i] > mobile) { 
      if (dir[a[i] - 1] == LEFT_TO_RIGHT) 
        dir[a[i] - 1] = RIGHT_TO_LEFT; 
      else if (dir[a[i] - 1] == RIGHT_TO_LEFT) 
        dir[a[i] - 1] = LEFT_TO_RIGHT; 
    } 
  } 
} 

vector<PackID> getOffsetSolution(vector<PackID> &solution) {
  vector<PackID> offsetSolution = solution;
  for (size_t i = 0; i < offsetSolution.size(); i++)
    offsetSolution[i] -= 1;

  return offsetSolution;
}

vector<vector<PackID>> complsearch::complsearch::solve(size_t nRobots, size_t robotCapacity, const Warehouse &warehouse) {

  // Create initial solution
  vector<PackID> solution;
  vector<bool> dir;
  size_t packages = warehouse.getPackageLocations().size();

  if (packages != nRobots * robotCapacity) {
    throw runtime_error("Complete search does only work when nRobots * robotCapacity == nPackages atm");
  }

  for (size_t i = 0; i < packages; i++) {
    solution.push_back(i+1);
    dir.push_back(RIGHT_TO_LEFT);
  }

  // Test all solutions and pick best
  // Johnson and Trotter algorithm. 
  int n = nRobots * robotCapacity;

  if (n > 13) {
    throw runtime_error("> 13! ... try less robots or less capacity");
  }


  int bestScore = evaluateSolutionTime(warehouse, getOffsetSolution(solution), nRobots, robotCapacity);
  vector<PackID> bestSolution = solution;

  int stop = fact(n);
  for (int i = 1; i < stop; i++) {
    nextSolution(solution, dir, n);
    int score = evaluateSolutionTime(warehouse, getOffsetSolution(solution), nRobots, robotCapacity);
    if (score < bestScore) {
      bestScore = score;
      bestSolution = getOffsetSolution(solution);
    }

    cout << i << "/" << stop << "            \r";
  }

  vector<vector<PackID>> fsolution;
  vector<PackID> batch;
  for (size_t i = 0; i < nRobots * robotCapacity; i++) {
    if (i % robotCapacity == 0 && i != 0) {
      fsolution.push_back(batch);
      batch.clear();
    }
    if (bestSolution[i] != -1) {
      batch.push_back(bestSolution[i]);
    }
  }

  if (fsolution.size() != nRobots)
    fsolution.push_back(batch);

  return fsolution;
}

