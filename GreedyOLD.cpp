#include "Greedy.h"

Greedy::Greedy(ProblemInstance* pi) {
    this->problem = pi;
    this->processorsTimes = new uint32_t[problem->numOfProcessors]{0};
    this->processorsArray = new std::vector<uint32_t>[problem->numOfProcessors];
}

void Greedy::solve() {
        for(uint32_t i = 0; i < problem->numOfTasks; i++) {
            uint32_t index = minIndexOf(processorsTimes, problem->numOfProcessors);
            processorsArray[index].push_back(problem->tasksArray[i]);
            processorsTimes[index] += problem->tasksArray[i];
        }
        tabPrint(processorsArray, problem->numOfProcessors);
}

int32_t minIndexOf(uint32_t* tab, uint32_t size) {
    int32_t minIndex = -1;
    for(int32_t i = 0; i < size; i++) {
        if(minIndex == -1) {
            minIndex = i;
        } else {
            if(tab[i] < tab[minIndex]) minIndex = i;
        }
    }
    return minIndex;
}

void tabPrint(std::vector<uint32_t> *tab, uint32_t size) {
    for(uint32_t i = 0; i < size; i++){
        std::cout << "Processor " << i << ": ";
        for(uint32_t k = 0; k < tab[i].size(); k++) {
            std::cout << tab[i][k] << " ";
        }
        std::cout<<std::endl;
    }
}