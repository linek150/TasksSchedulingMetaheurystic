#ifndef GENETIC_H
#define GENETIC_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Individual.h"

class Genetic: public SchedulingAlgorithm {
public:
    Genetic(ProblemInstance* pi);

    void solve();
    void setMethod(uint32_t method);
    void generatePopulation();

private:
    Individual* population;
    Individual* reproductionArray;

    uint32_t populationSize;
    uint32_t childrenNumber;
    uint32_t reproductionRatio;
    uint32_t fightGroupSize;
    uint32_t tourneyRatio;
    uint32_t mutationChance;
    uint32_t mutationRatio;
    uint32_t method;
    uint32_t* stopCondition;
    uint32_t reproductionGroupSize;

    void adjustParameters();
    void tourney();
    void rank();
    void crossover();
};

#endif /* !GENETIC_H */