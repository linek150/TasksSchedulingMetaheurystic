#include "Individual.h"


ProblemInstance* Individual::_problem ;
Individual::Individual(ProblemInstance* pi, uint32_t* genes){
    Individual::_problem = pi;
    this->genes=genes;
    findFitness();
}
Individual::Individual(uint32_t* genes)
{
    this->genes=genes;
    findFitness();
}
Individual::Individual()
{
    this->genes = new uint32_t[Individual::_problem->numOfTasks]{0};
    fitness = 0;
}
Individual::Individual(const Individual& a) {
    this->fitness = a.fitness;
    this->genes = new uint32_t[Individual::_problem->numOfTasks]{ 0 };
    for (uint32_t i = 0; i < Individual::_problem->numOfTasks; i++)
    {
        this->genes[i] = a.genes[i];
    }
   
}
Individual::~Individual()
{
    delete[] genes;
}

void Individual::operator=(const Individual&   individualToCopy)
{
    for (uint32_t i = 0; i < Individual::_problem ->numOfTasks; i++)
    {
        this->genes[i]=individualToCopy.genes[i];
    }
    this->fitness = individualToCopy.fitness;
}

void Individual::findFitness()
{
    uint32_t* tmp = new uint32_t[Individual::_problem ->numOfProcessors]{0};
    fitness=0;
    for(int i = 0; i < Individual::_problem ->numOfTasks; i++){
        tmp[genes[i]]+=Individual::_problem ->tasksArray[i];
    }
    for(int i = 0; i < Individual::_problem ->numOfProcessors; i++){
        if(fitness<tmp[i]){
            fitness=tmp[i];
        }
    }
    delete[] tmp;
}

void Individual::copyGenes(Individual& parent,uint32_t numOfGenes,uint32_t* order,uint32_t offset)
{
    for (uint32_t gene = offset; gene < numOfGenes + offset; gene++)
    {
        this->genes[gene]=parent.genes[order[gene]];
    }
}
void Individual::copyGenes(uint32_t* genes)
{
    for (uint32_t gene = 0; gene < Individual::_problem ->numOfTasks; gene++)
    {
        this->genes[gene]=genes[gene];
    }
    findFitness();  
}