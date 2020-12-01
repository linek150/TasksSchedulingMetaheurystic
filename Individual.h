#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <vector>
#include "ProblemInstance.h"

class Individual {
public:
    Individual(ProblemInstance* pi, uint32_t* genes);
    Individual(uint32_t* genes);
    Individual();
    ~Individual();


    void operator =(const Individual& individualToCopy);
    
    void findFitness();
    void copyGenes(Individual& parent,uint32_t numOfGenes,uint32_t* order,uint32_t offset=0);
    void copyGenes(uint32_t* genes);
    uint32_t fitness;
    uint32_t* genes;
private:
    static ProblemInstance* _problem;
friend class Genetic;
};

#endif /* !INDIVIDUAL_H */