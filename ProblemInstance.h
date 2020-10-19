#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

#include <random>
class ProblemInstance{
    public:
        ProblemInstance(uint32_t numOfTasks,uint32_t numOfProcessors,uint32_t maxTaskTime,uint32_t minTaskTime=1);

        uint32_t numOfTasks;
        uint32_t numOfProcessors;
        uint32_t* tasksArray;

        void generateTasksArray();
        bool setTaskTime(uint32_t maxTaskTime,uint32_t minTaskTime=1);

    private:
        std::mt19937_64 _rngEngine;
        std::uniform_int_distribution<uint32_t> _rngDistribution;

};

#endif /* !PROBLEM_INSTANCE_H */