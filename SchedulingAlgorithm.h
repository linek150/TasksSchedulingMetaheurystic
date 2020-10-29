#ifndef SCHEDULINGALGORITHM_H
#define SCHEDULINGALGORITHM_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"

class SchedulingAlgorithm{
public:
    SchedulingAlgorithm(ProblemInstance* pi);
    ~SchedulingAlgorithm();

    virtual void solve() = 0;
protected:
    ProblemInstance* problem;
    uint32_t* processorsTimes;
    std::vector<uint32_t>* processorsArray;
};

int32_t minIndexOf(uint32_t* tab, uint32_t size);
void tabPrint(std::vector<uint32_t> *tab, uint32_t size);

#endif /* !SCHEDULINGALGORITHM_H */