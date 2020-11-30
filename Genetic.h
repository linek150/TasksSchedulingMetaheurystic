#ifndef GENETIC_H
#define GENETIC_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"
#include "Individual.h"
#include <random>
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
                 uint32_t populationSize=100,
                uint32_t numOfChildren=2,
                uint32_t numOfParents=2,
                uint32_t stopValue=0,
                float tourneyRatio=0.5,
                float mutationChance=0.01,
                float mutationRatio=0.1,
                float reproductionRatio=0.5,
                Method method=Rank,
                Condition stopCondition=Time);
    ~Genetic();
    void solve();
    void setMethod(Method method);
    void generatePopulation();

    
private:
    Individual* population;
    Individual* repArr;
    Individual* _siblings;
    uint32_t* _childGenes;

    ProblemInstance* problem;

    uint32_t populationSize;
    uint32_t numOfChildren;
    uint32_t numOfParents;
    uint32_t fightGroupSize;
    uint32_t stopValue;
    uint32_t repGroupSize;
    float tourneyRatio;
    float mutationChance;
    float mutationRatio;
    float reproductionRatio;
    Method method;
    Condition stopCondition; 

    std::mt19937_64* _rngEngine;
    std::uniform_int_distribution<uint32_t>* _rngProcDistribution;
    uint32_t* _repGrpSizeOrd;//order of crossover
    uint32_t* _repGenOrd;
    uint32_t _parentPart;
    uint32_t _remGenes;

    static uint32_t* genOrderArr(uint32_t numOfElements);

    void adjustParameters();
    void tourney();
    void rank();
    void crossover();
};


#endif /* !GENETIC_H */