#ifndef GREEDY_H
#define GREEDY_H

#include <iostream>
#include <vector>

#include "ProblemInstance.h"
#include "SchedulingAlgorithm.h"

class Greedy: public SchedulingAlgorithm {
public:
    Greedy(ProblemInstance* pi);

    void solve();
};

#endif /* !GREEDY_H */