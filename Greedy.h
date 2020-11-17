#ifndef GREEDY_H
#define GREEDY_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"

class Greedy: public SchedulingAlgorithm {
public:
    Greedy(ProblemInstance* pi);
    ~Greedy();

    void solve();
};

#endif /* !GREEDY_H */