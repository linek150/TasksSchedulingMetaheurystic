#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <vector>
#include "ProblemInstance.h"

class Individual {
public:
    Individual(ProblemInstance* pi, uint32_t* genes);
    Individual();
    ~Individual();


    Individual& operator =(const Individual& individualToCopy);// TODO deep assign - Piotr
    static ProblemInstance* pi;
    void findFitness();
    void copyGenes(Individual& parent,uint32_t numOfGenes,uint32_t* order,uint32_t offset=0);

    ProblemInstance* problem;
    uint32_t fitness;
    uint32_t* genes;
};

#endif /* !INDIVIDUAL_H */