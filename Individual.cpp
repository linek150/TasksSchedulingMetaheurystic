#include "Individual.h"


ProblemInstance* Individual::_staticProblem;
Individual::Individual(ProblemInstance* pi, uint32_t* genes){
    this->problem = pi;
    this->genes=genes;
    findFitness();
}
Individual::Individual(uint32_t* genes)
{
    this->genes=genes;
}
Individual::Individual()
{
    this->genes=new uint32_t[Individual::_staticProblem->numOfTasks]();
}
Individual::~Individual()
{
    delete[] genes;
}

void Individual::operator=(const Individual&   individualToCopy)
{
    for (uint32_t i = 0; i < Individual::_staticProblem->numOfTasks; i++)
    {
        this->genes[i]=individualToCopy.genes[i];
    }
}

void Individual::findFitness()
{
    uint32_t* tmp = new uint32_t[problem->numOfProcessors]{0};
    fitness=0;
    for(int i = 0; i < problem->numOfTasks; i++){
        tmp[genes[i]]+=problem->tasksArray[i];
    }
    for(int i = 0; i < problem->numOfProcessors; i++){
        if(fitness<tmp[i]){
            fitness=tmp[i];
        }
    }
    delete[] tmp;
}

void Individual::copyGenes(Individual& parent,uint32_t numOfGenes,uint32_t* order,uint32_t offset)
{
    for (uint32_t gene = offset; gene < numOfGenes; gene++)
    {
        this->genes[gene]=parent.genes[order[gene]];
    }
}
void Individual::copyGenes(uint32_t* genes)
{
    for (uint32_t gene = 0; gene < Individual::_staticProblem->numOfTasks; gene++)
    {
        this->genes[gene]=genes[gene];
    }   
}