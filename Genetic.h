#ifndef GENETIC_H
#define GENETIC_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Individual.h"
enum Method: uint32_t
{
        Turney,Rank,Turney_Rank
};
enum Condition: uint32_t
{
        Time,Iteration
};
class Genetic: public SchedulingAlgorithm {
public:
    Genetic(ProblemInstance *pi,
                 uint32_t populationSize=0,
                uint32_t childrenNumber=0,
                uint32_t stopValue=0,
                float tourneyRatio=-1,
                float mutationChance=-1,
                float mutationRatio=-1,
                float reproductionRatio=-1,
                Method method=Rank,
                Condition stopCondition=Time);
    
    void solve();
    void setMethod(Method method);
    void generatePopulation();
    
private:
    Individual* population;
    Individual* reproductionArray;

    uint32_t populationSize=100;
    uint32_t childrenNumber=2;
    uint32_t fightGroupSize=0;
    uint32_t stopValue=0;
    uint32_t reproductionGroupSize=0;
    float tourneyRatio=0.5;
    float mutationChance=0.01;
    float mutationRatio=0.1;
    float reproductionRatio=0.5;
    Method method=Rank;
    Condition stopCondition=Time; 
    void adjustParameters();
    void tourney();
    void rank();
    void crossover();
};


#endif /* !GENETIC_H */