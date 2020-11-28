#include "Genetic.h"
#include <fstream>
void Genetic::setMethod(Method method)
{
    this->method = method;
    adjustParameters();
}
Genetic::Genetic(ProblemInstance *pi,
                 uint32_t populationSize,
                 uint32_t childrenNumber,
                 uint32_t stopValue,
                 float tourneyRatio,
                 float mutationChance,
                 float mutationRatio,
                 float reproductionRatio,
                 Method method = Rank,
                 Condition stopCondition = Time) : SchedulingAlgorithm(pi)
{
    this->populationSize=populationSize;
    this->childrenNumber=childrenNumber;
    this->stopValue=stopValue;
    this->tourneyRatio=tourneyRatio;
    this->mutationChance=mutationChance;
    this->reproductionRatio=reproductionRatio;
    this->method=method;
    this->stopCondition=stopCondition;
    adjustParameters();
}
void Genetic::adjustParameters()
{

    reproductionGroupSize=(numOfParents/childrenNumber)*reproductionRatio*populationSize;
    if(reproductionGroupSize%numOfParents!=0)reproductionGroupSize--;
    if (this->method!=Rank)
    {
        if(this->method==Turney)
        {
            fightGroupSize=ceil(populationSize/reproductionGroupSize);
        }
        if(this->method==Turney_Rank)
        {
            tourneyRatio=reproductionRatio+(1-reproductionRatio)*tourneyRatio;
            fightGroupSize=ceil(populationSize/(reproductionGroupSize/tourneyRatio));
        }
    }   
}
void Genetic::generatePopulation()
{
    delete [] this->population;
    this->population=new Individual[this->populationSize](this->problem);
}
void Genetic::crossover()
{
    for (int  i = 0; i < reproductionGroupSize/numOfParents; i++)
    {
        
    }
    
}