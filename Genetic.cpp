#include "Genetic.h"
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Individual.h"
void Genetic::setMethod(Method method)
{
    this->method = method;
    adjustParameters();
}
Genetic::Genetic(ProblemInstance *pi,
                 uint32_t populationSize,
                 uint32_t numOfChildren,
                 uint32_t numOfParents,
                 uint32_t stopValue,
                 float tourneyRatio,
                 float mutationChance,
                 float mutationRatio,
                 float reproductionRatio,
                 Method method = Rank,
                 Condition stopCondition = Time) : SchedulingAlgorithm(pi)
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
    Individual::pi = this->problem;

    this->_siblings = new Individual[this->numOfChildren];
    this->_childGenes = new uint32_t[pi->numOfTasks];

    this->_rngEngine = new std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    this->_repGrpSizeOrd = genOrderArr(this->repGroupSize);
    this->_repGenOrd = genOrderArr(this->problem->numOfTasks);
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
        }
    }
    this->_parentPart = this->problem->numOfTasks / numOfParents;
    this->_remGenes = this->problem->numOfTasks % numOfParents;
}
void Genetic::generatePopulation()
{
    delete[] this->population;
    this->population = new Individual[this->populationSize];
}
void Genetic::crossover()
{
    //randomize parent selection
    std::shuffle(_repGrpSizeOrd, _repGrpSizeOrd + repGroupSize - 1, _rngEngine);
    //randomize genes selection
    std::shuffle(_repGenOrd, _repGenOrd + problem->numOfTasks - 1, _rngEngine);
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
        for (uint32_t idx=startIdx; startIdx < finalIdx;idx++ )
        {
            population[idx]=_siblings[idx-startIdx];
        }
        
    }
}