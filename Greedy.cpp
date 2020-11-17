#include "Greedy.h"

Greedy::Greedy(ProblemInstance* pi):SchedulingAlgorithm(pi) {
}

Greedy::~Greedy() {
    delete[] processorsArray;
    delete[] processorsTimes;
}

void Greedy::solve() {
        for(uint32_t i = 0; i < problem->numOfTasks; i++) {
            uint32_t index = minIndexOf(processorsTimes, problem->numOfProcessors);
            processorsArray[index].push_back(problem->tasksArray[i]);
            processorsTimes[index] += problem->tasksArray[i];
        }
        tabPrint(processorsArray, problem->numOfProcessors);
}