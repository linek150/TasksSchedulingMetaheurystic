#include "Individual.h"

Individual::Individual(ProblemInstance* pi, uint32_t* genes){
    this->problem = pi;
    this->genes=genes;
    findFitness();
}

Individual::~Individual(){
    delete[] genes;
}

void Individual::findFitness(){
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