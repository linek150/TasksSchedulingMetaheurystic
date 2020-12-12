#include "SchedulingAlgorithm.h"

SchedulingAlgorithm::SchedulingAlgorithm(ProblemInstance* pi) {
    this->problem = pi;
    this->processorsTimes = new uint32_t[problem->numOfProcessors]{0};
    this->processorsArray = new std::vector<uint32_t>[problem->numOfProcessors];
}
SchedulingAlgorithm::~SchedulingAlgorithm() {
    delete[] processorsTimes;
    for(int i=0; i<problem->numOfProcessors; i++) {
        processorsArray[i].clear();
        std::vector<uint32_t>().swap(processorsArray[i]);
    }
    delete[] processorsArray;
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

void maxCMAX(std::vector<uint32_t> *tab, uint32_t size) {
    uint32_t* tmp = new uint32_t[size]{0};
    uint32_t fit = 0;
    for(uint32_t i = 0; i < size; i++){
        for(uint32_t k = 0; k < tab[i].size(); k++) {
            tmp[i]+=tab[i][k];
        }
        if (tmp[i] > fit) { fit = tmp[i]; }
    }
    std::cout << "cMAX = " << fit << std::endl;
}