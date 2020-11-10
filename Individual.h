#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <vector>

class Individual {
public:
    //Individual();

    void findFitness();

    uint32_t fitness;
    uint32_t* genes;
};

#endif /* !INDIVIDUAL_H */