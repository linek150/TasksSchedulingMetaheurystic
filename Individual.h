#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <vector>
#include "ProblemInstance.h"

class Individual {
public:
    Individual(ProblemInstance* pi, uint32_t* genes);
    ~Individual();

    void findFitness();

    ProblemInstance* problem;
    uint32_t fitness;
    uint32_t* genes;
};

#endif /* !INDIVIDUAL_H */