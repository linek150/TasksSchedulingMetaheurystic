#include <iostream>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Greedy.h"

using namespace std;

int main() {

    ProblemInstance p(10,3,15,1);
    Greedy g(&p);
    g.solve();
    p.genFile("Test.txt");

    //delete &g;
    //delete &p;
    return 0;
}