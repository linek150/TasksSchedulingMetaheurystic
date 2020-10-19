#include "ProblemInstance.h"


bool ProblemInstance::setTaskTime(uint32_t maxTaskTime, uint32_t minTaskTime )
{
    if (maxTaskTime>minTaskTime)
    {
        this->_rngDistribution=std::uniform_int_distribution<uint32_t>(minTaskTime,maxTaskTime);
        return true;
    }
    else return false;
}


void ProblemInstance::generateTasksArray()
{
    if (this->tasksArray!=NULL)
    {
        delete [] this->tasksArray;
    }
    this->tasksArray=new uint32_t[this->numOfTasks];
    for (uint32_t i = 0; i < this->numOfTasks; i++)
    {
        this->tasksArray[i]=this->_rngDistribution(this->_rngEngine);
    }
    
}
ProblemInstance::ProblemInstance(uint32_t numOfTasks,uint32_t numOfProcessors,uint32_t maxTaskTime,uint32_t minTaskTime)
{
    this->numOfProcessors=numOfProcessors;
    this->numOfTasks=numOfTasks;
    std::random_device rd;
    this->_rngEngine=std::mt19937_64(rd());
    this->_rngDistribution=std::uniform_int_distribution<uint32_t>(minTaskTime,maxTaskTime);
    generateTasksArray();
}