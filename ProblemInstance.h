#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

#include <random>
class ProblemInstance{
    public:
        ProblemInstance(uint32_t numOfTasks,uint32_t numOfProcessors,uint32_t maxTaskTime,uint32_t minTaskTime=1);
        ProblemInstance(const char* fileName);
        uint32_t numOfTasks;
        uint32_t numOfProcessors;
        uint32_t* tasksArray;


        uint32_t cMax();
        void generateTasksArray();
        bool setTaskTime(uint32_t maxTaskTime,uint32_t minTaskTime=1);
        void loadInstanceFromFile(const char* fileName);
        void genFile(const char* fileName);

    private:
        void distribiuteEvenlyOnProcessors(uint32_t* tasks, uint32_t numOfTasks, std::vector<uint32_t>* processors);
        void makeTasksArray(std::vector<uint32_t>* processors);
        void commonInit(uint32_t maxTaskTime=100,uint32_t minTaskTime=1);
        uint32_t* genNumOfTasks(uint32_t numOfTasks);
        uint32_t alignProcessors(std::vector<uint32_t>* processors);
        uint32_t _cMax;
        std::mt19937_64 _rngEngine;
        std::uniform_int_distribution<uint32_t> _rngDistribution;

};
void printArrayOfVector(std::vector<uint32_t>* processorsArray,uint32_t arraySize);
void printArray(uint32_t* array, uint32_t arraySize);
#endif /* !PROBLEM_INSTANCE_H */