#include <iostream>
#include <vector>

using namespace std;

int minIndexOf(unsigned int* tab, int size) {
    int minIndex = -1;
    for(int i = 0; i < size; i++) {
        if(minIndex == -1) {
            minIndex = i;
        } else {
            if(tab[i] < tab[minIndex]) minIndex = i;
        }
    }
    return minIndex;
}

void tabPrint(vector<unsigned int> *tab, unsigned int size) {
    for(int i = 0; i < size; i++){
        cout << "Processor " << i << ": ";
        for(int k = 0; k < tab[i].size(); k++) {
            cout << tab[i][k] << " ";
        }
        cout<<endl;
    }
}


class ProblemInstance {
public:
    unsigned int numOfProcessors = 5;
    unsigned int numOfTasks = 8;
    unsigned int taskArray[8] = {5,3,12,8,1,7,20,9};
};

class Greedy {
public:
    ProblemInstance* problem;
    unsigned int* processorsTimes;
    vector<unsigned int>* processorsArray;

    Greedy(ProblemInstance* pi) {
        this->problem = pi;
        this->processorsTimes = new unsigned int[problem->numOfProcessors]{0};
        this->processorsArray = new vector<unsigned int>[problem->numOfProcessors];
    }

    void solve() {
        for(int i = 0; i < problem->numOfTasks; i++) {
            unsigned int index = minIndexOf(processorsTimes, problem->numOfProcessors);
            processorsArray[index].push_back(problem->taskArray[i]);
            processorsTimes[index] += problem->taskArray[i];
        }
        tabPrint(processorsArray, problem->numOfProcessors);
    }

};

int main() {

    ProblemInstance p;
    Greedy g(&p);
    g.solve();

    return 0;
}