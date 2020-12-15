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
        Tourney,Rank,Tourney_Rank
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
    Individual* _population;
    Individual* _repArr;
    Individual* _siblings;
    Individual* _tourArr;
    uint32_t* _childGenes;
    uint32_t* _tourPopArr;


    uint32_t _populationSize;
    uint32_t _numOfChildren;
    uint32_t _numOfParents;
    uint32_t _fightGroupSize;
    uint32_t _tourGroupSize;
    uint32_t _stopValue;
    uint32_t _repGroupSize;
    float _tourneyRatio;
    float _mutationChance;
    float _mutationRatio;
    float _reproductionRatio;
    Method _method;
    Condition _stopCondition; 

    std::mt19937_64* _rngEngine;
    std::uniform_int_distribution<uint32_t>* _rngProcDistribution;
    std::uniform_int_distribution<uint32_t>* _rngCrossCut;
    std::uniform_real_distribution<float>* _mutationRandGen;
    uint32_t* _repGrpSizeOrd;//order of crossover
    uint32_t* _repGenOrd;
    uint32_t* _crossOrder;
    uint32_t _parentPart;
    uint32_t _remGenes;
    uint32_t _numOfMutation;
    uint32_t _extendedPopulationSize;

    static uint32_t* genOrderArr(uint32_t numOfElements);

    void solveIterated();
    void adjustParameters();
    void tourney();
    void rank();
    void crossover();
    uint32_t index(std::uniform_int_distribution<uint32_t>* ran, uint32_t idx);
    void crossover2();
    void sortPopulation();
    void sortTourArr();
    void printParameters();
    void printIntArray(uint32_t *array, uint32_t size);
    void mutation(uint32_t* tab);
    void mutation2(uint32_t* tab);



    friend int main();//remove after debug
    void printIndividualsArray(Individual *tab, uint32_t size);
};


#endif /* !GENETIC_H */