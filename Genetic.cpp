#include "Genetic.h"
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Individual.h"
#include <time.h>

Genetic::Genetic(ProblemInstance *pi,
                 uint32_t populationSize,
                 uint32_t numOfChildren,
                 uint32_t numOfParents,
                 uint32_t stopValue,
                 float tourneyRatio,
                 float mutationChance,
                 float mutationRatio,
                 float reproductionRatio,
                 Method method,
                 Condition stopCondition)
                : SchedulingAlgorithm(pi)
{
    this->populationSize = populationSize;
    this->numOfChildren = numOfChildren;
    this->numOfParents = numOfParents;
    this->stopValue = stopValue;
    this->tourneyRatio = tourneyRatio;
    this->mutationChance = mutationChance;
    this->reproductionRatio = reproductionRatio;
    this->method = method;
    this->stopCondition = stopCondition;
    this->problem = pi;
    adjustParameters();
    
    Individual::_staticProblem = this->problem;

    this->repArr=new Individual[this->repGroupSize];
    this->population=nullptr;

    this->_siblings = new Individual[this->numOfChildren];
    this->_childGenes = new uint32_t[pi->numOfTasks];

    this->_rngEngine = new std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    this->_rngProcDistribution=new std::uniform_int_distribution<uint32_t>(1,this->problem->numOfProcessors+1);
    this->_repGrpSizeOrd = genOrderArr(this->repGroupSize);
    this->_repGenOrd = genOrderArr(this->problem->numOfTasks);
}
void Genetic::setMethod(Method method)
{
    this->method = method;
    adjustParameters();
}
uint32_t *Genetic::genOrderArr(uint32_t numOfElements)
{
    uint32_t *arr = new uint32_t[numOfElements];
    for (uint32_t i = 0; i < numOfElements; i++)
    {
        arr[i] = i;
    }
    return arr;
}
void Genetic::adjustParameters()
{

    repGroupSize = (numOfParents / numOfChildren) * reproductionRatio * populationSize;
    if (repGroupSize % numOfParents != 0)
        repGroupSize--;
    if (this->method != Rank)
    {
        if (this->method == Turney)
        {
            fightGroupSize = ceil(populationSize / repGroupSize);
        }
        if (this->method == Turney_Rank)
        {
            tourneyRatio = reproductionRatio + (1 - reproductionRatio) * tourneyRatio;
            fightGroupSize = ceil(populationSize / (repGroupSize / tourneyRatio));
            _tourGroupSize = populationSize/fightGroupSize + (populationSize % fightGroupSize != 0);
        }
    }
    this->_parentPart = this->problem->numOfTasks / numOfParents;
    this->_remGenes = this->problem->numOfTasks % numOfParents;
}
void Genetic::generatePopulation()
{
    delete[] this->population;
    this->population = new Individual[this->populationSize];
    uint32_t* genes;
    genes=new uint32_t[this->problem->numOfTasks];
    for ( uint32_t individual = 0; individual < this->populationSize; individual++)
    {
        for (uint32_t gene = 0; gene<this->problem->numOfTasks; gene++)
        {
            genes[gene]=(*this->_rngProcDistribution)(*this->_rngEngine);
        }
        population[individual].copyGenes(genes);
    }
    delete[] genes;
}
void Genetic::crossover()
{
    //randomize parent selection
    std::shuffle(_repGrpSizeOrd, _repGrpSizeOrd + repGroupSize - 1, *_rngEngine);
    //randomize genes selection
    std::shuffle(_repGenOrd, _repGenOrd + problem->numOfTasks - 1, *_rngEngine);
    //genes that need to be taken from first parents coused by numOfTasks%numOfParents!=0
    uint32_t remGenes = this->_remGenes;
    //additional offset coused by numOfTasks%numOfParents!=0
    uint32_t addOffset = 0; 
    uint32_t curParent = 0;
    for (int curParrents = 0; curParrents < repGroupSize / numOfParents; curParrents++)
    {
        //generate siblings
        for (uint32_t sibling = 0; sibling < numOfChildren; sibling++)
        {
            //take part of genes from every parent
            for (uint32_t parent = 0; parent < numOfParents; parent++)
            {
                curParent = _repGrpSizeOrd[curParrents * numOfParents + parent];
                //take one more gene if needed from a few first parents
                if (remGenes)
                {
                    _siblings[sibling].copyGenes(repArr[curParent],
                                               _parentPart + 1,
                                               _repGenOrd,
                                               _parentPart * parent + addOffset);
                    remGenes--;
                    addOffset++;
                }
                else
                {
                    _siblings[sibling].copyGenes(repArr[curParent],
                                               _parentPart,
                                               _repGenOrd,
                                               _parentPart * parent + addOffset);
                }
            }
            remGenes = this->_remGenes;
            addOffset = 0;
        }
        uint32_t startIdx=populationSize-1-(curParrents+1)*numOfChildren;
        uint32_t finalIdx=startIdx+numOfChildren;
        //Raplace last postion from sorted population by new individuals
        for (uint32_t idx=startIdx; idx < finalIdx;idx++ )
        {
            population[idx]=_siblings[idx-startIdx];
        }
        
    }
}

void Genetic::sortPopulation()
{
    std::sort(population, population + populationSize,
    [](Individual const & a, Individual const & b) -> bool { return a.fitness < b.fitness; }
    );
}

void Genetic::sortTourArr()
{
    std::sort(tourArr, tourArr + _tourGroupSize,
    [](Individual const & a, Individual const & b) -> bool { return a.fitness < b.fitness; }
    );
}

void Genetic::solve()
{
    switch (stopCondition)
    {
    case Time: {

        const std::chrono::time_point start = std::chrono::high_resolution_clock::now();
        const std::chrono::time_point end = start + std::chrono::seconds(stopValue);

        while(std::chrono::high_resolution_clock::now() <= end) {
            sortPopulation();
            solveIterated();
        }
        break;
    }
    case Iteration: {
        for (uint32_t i = 0; i <= stopValue; i++){
            sortPopulation();
            solveIterated();
        }
        break;
    }
    }
    sortPopulation();
    printf("Najlepszy cMAX wynosi: %u", population[0].fitness);

}

void Genetic::solveIterated()
{
    switch (method)
    {
    case Rank:
        rank();
        break;
    case Turney:
        tourney();
        break;
    case Turney_Rank:
        tourney();
        rank();
        break;
    }
    crossover();

    /*if (method == Turney || method == Turney_Rank){
        tourney();
    }
    if (method == Rank || method == Turney_Rank){
        rank();
    }
    crossover();*/
}

void Genetic::rank()
{
    switch (method)
    {
    case Rank:{
        this->repArr = new Individual[repGroupSize];
        std::copy(population, population + repGroupSize, repArr);
        break;
    }
    case Turney_Rank: {
        this->repArr = new Individual[repGroupSize];
        sortTourArr();
        std::copy(tourArr, tourArr + repGroupSize, repArr);
        delete[] tourArr;
        break;
    }
    }
}

void Genetic::tourney()
{
    // Generate outcome array
    Individual* tourOutArrName;
    if(method == Turney){
        this->repArr = new Individual[repGroupSize];
        tourOutArrName = repArr;
    }
    else if(method == Turney_Rank){
        this->tourArr = new Individual[_tourGroupSize];
        tourOutArrName = tourArr;
    }

    // Generate array to fight
    uint32_t* tourPopArr = genOrderArr(populationSize);

    /*uint32_t* tourPopArr = new uint32_t[populationSize];
    for (uint32_t i = 0; i < populationSize; i++){
        tourPopArr[i] = i;
    }*/

    // Shuffle array to fight
    std::mt19937_64* tourneyEngine = new std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::shuffle(tourPopArr, tourPopArr + populationSize, *tourneyEngine);

    // Fight
    for (uint32_t i = 0; i < populationSize; i++) {
        if (i % fightGroupSize == 0) {
            tourOutArrName[i/fightGroupSize] = population[tourPopArr[i]];
        }
        else {
            if (population[tourPopArr[i]].fitness < tourOutArrName[i/fightGroupSize].fitness) {
                tourOutArrName[i/fightGroupSize] = population[tourPopArr[i]];
            }
        }
    }

    delete[] tourPopArr;
    delete tourneyEngine;
}


Genetic::~Genetic()
{
    delete [] population;
    delete [] repArr;
    delete [] _siblings;
    delete [] _childGenes;
    delete _rngEngine;
    delete _rngProcDistribution;
    delete [] _repGrpSizeOrd;
    delete [] _repGenOrd;
}
