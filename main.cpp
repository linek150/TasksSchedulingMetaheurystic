#include <iostream>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Greedy.h"
#include "Genetic.h"

using namespace std;

int main() {

    //ProblemInstance p("m10n200.txt");
    //Greedy g(&p);
    //g.solve();

    //towrzymy zmienne o informatywnych nazwach 
    /*ProblemInstance p(10,3,15,1);
    Greedy g(&p);
    g.solve();
    p.genFile("Test.txt");*/


    ProblemInstance k("m50.txt");
    //ProblemInstance k(100, 4, 60, 1);
    Genetic j(&k, 2000, 2, 2, 2000, 0.5, 0.95, 0.1, 0.4, Rank, Iteration);
    Greedy g(&k);
    j.solve();
    g.solve();

    
    return 0;
}