#include <iostream>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Greedy.h"
#include "Genetic.h"

using namespace std;

int main() {

    ProblemInstance p(10,3,15,1);
    Greedy g(&p);
    g.solve();
    p.genFile("Test.txt");


    ProblemInstance k(7,3,5,1);
    Genetic j(&k, 7, 2, 2, 10, 0.5, 0.1, 0.1, 0.5, Rank, Iteration);
    j.generatePopulation();
    j.solve();

    
    return 0;
}