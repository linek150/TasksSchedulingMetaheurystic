#ifndef GREEDY_H
#define GREEDY_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"

class Greedy{
public:
    Greedy(ProblemInstance* pi);

    ProblemInstance* problem;
    uint32_t* processorsTimes;
    std::vector<uint32_t>* processorsArray;

    void solve();
};

int32_t minIndexOf(uint32_t* tab, uint32_t size);
void tabPrint(std::vector<uint32_t> *tab, uint32_t size);

#endif /* !GREEDY_H */