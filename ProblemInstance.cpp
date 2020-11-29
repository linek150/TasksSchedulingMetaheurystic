#include "ProblemInstance.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

ProblemInstance::ProblemInstance(uint32_t numOfTasks, uint32_t numOfProcessors, uint32_t maxTaskTime, uint32_t minTaskTime)
{
    this->commonInit(maxTaskTime,minTaskTime);
    this->numOfProcessors = numOfProcessors;
    this->numOfTasks = numOfTasks;
    generateTasksArray();
}
ProblemInstance::ProblemInstance(const char* fileName)
{
    this->commonInit();
    this->loadInstanceFromFile(fileName);
}
void ProblemInstance::commonInit(uint32_t maxTaskTime,uint32_t minTaskTime)
{
    this->tasksArray=nullptr;

    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    this->_rngEngine.seed((unsigned long)seed);

    this->_rngDistribution = std::uniform_int_distribution<uint32_t>(minTaskTime, maxTaskTime);
}
bool ProblemInstance::setTaskTime(uint32_t maxTaskTime, uint32_t minTaskTime)
{
    if (maxTaskTime > minTaskTime)
    {
        this->_rngDistribution = std::uniform_int_distribution<uint32_t>(minTaskTime, maxTaskTime);
        return true;
    }
    else
        return false;
}
void ProblemInstance::generateTasksArray()
{
    uint32_t *tasks;
    uint32_t numOfTasks = this->numOfTasks - this->numOfProcessors;
    std::vector<uint32_t> *processorsArray = new std::vector<uint32_t>[this->numOfProcessors];
    tasks = genNumOfTasks(numOfTasks);
    this->distribiuteEvenlyOnProcessors(tasks, numOfTasks, processorsArray);
    delete[] tasks;
    this->_cMax = this->alignProcessors(processorsArray);
    makeTasksArray(processorsArray);
    delete[] processorsArray;
}
uint32_t *ProblemInstance::genNumOfTasks(uint32_t numOfTasks)
{
    uint32_t *tasks = new uint32_t[numOfTasks];
    for (uint32_t i = 0; i < numOfTasks; i++)
    {
        tasks[i] = this->_rngDistribution(this->_rngEngine);
    }
    return tasks;
}
void ProblemInstance::distribiuteEvenlyOnProcessors(uint32_t *tasks, uint32_t numOfTasks, std::vector<uint32_t> *processors)
{
    for (uint32_t i = 0; i < numOfTasks; i++)
    {
        processors[i % this->numOfProcessors].push_back(tasks[i]);
    }
}
uint32_t ProblemInstance::alignProcessors(std::vector<uint32_t> *processors)
{
    //find tasks duration of every processor
    uint32_t *processorTasksDuration = new uint32_t[this->numOfProcessors]();
    for (uint32_t processor = 0; processor < this->numOfProcessors; processor++)
    {
        for (uint32_t task = 0; task < processors[processor].size(); task++)
        {
            processorTasksDuration[processor] += processors[processor].at(task);
        }
    }
    uint32_t maxDurProcessor = *std::max_element(processorTasksDuration, processorTasksDuration + this->numOfProcessors);
    uint32_t maxDurProcessorIdx;
    //Find processor that needs the longest time
    for(uint32_t i=0;i<this->numOfProcessors;i++)
    {
        if(processorTasksDuration[i]==maxDurProcessor)maxDurProcessorIdx=i;
    }
    //Add one task to processor with the longest execution time
    uint32_t aligningTaskDur = this->_rngDistribution(this->_rngEngine);
    uint32_t cMax = processorTasksDuration[maxDurProcessorIdx] + aligningTaskDur;
    processors[maxDurProcessorIdx].push_back(aligningTaskDur);
    //Add one task to every other processor to align them to cMax
    for (uint32_t processor = 0; processor < this->numOfProcessors; processor++)
    {
        if (!(processor == maxDurProcessorIdx))
        {
            aligningTaskDur = cMax - processorTasksDuration[processor];
            processors[processor].push_back(aligningTaskDur);
        }
    }
    delete[] processorTasksDuration;
    return cMax;
}
void ProblemInstance::makeTasksArray(std::vector<uint32_t> *processors)
{
    delete[] this->tasksArray;
    this->tasksArray = new uint32_t[this->numOfTasks];
    uint32_t taskIdx = 0;
    for (uint32_t processorIdx = 0; processorIdx < this->numOfProcessors; processorIdx++)
    {
        while (!processors[processorIdx].empty())
        {
            this->tasksArray[taskIdx] = processors[processorIdx].back();
            processors[processorIdx].pop_back();
            taskIdx++;
        }
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (this->tasksArray, this->tasksArray + this->numOfTasks - 1, this->_rngEngine);
    
}
uint32_t ProblemInstance::cMax()
{
    return this->_cMax;
}
void ProblemInstance::genFile(const char* fileName)
{
    std::ofstream file;
    file.open(fileName);
    file<<this->numOfProcessors<<std::endl;
    file<<this->numOfTasks<<std::endl;
    for (uint32_t i = 0; i < this->numOfTasks; i++)
    {
       file<<this->tasksArray[i]<<std::endl;
    }
    file<<this->_cMax;
    file.close();
}
void ProblemInstance::loadInstanceFromFile(const char* fileName)
{
    std::ifstream file;
    file.open(fileName);
    file>>this->numOfProcessors;
    file>>this->numOfTasks;
    if(this->tasksArray==nullptr)
    {
        this->tasksArray=new uint32_t[this->numOfTasks];
    }
    for (uint32_t i = 0; i < this->numOfTasks; i++)
    {
       file>>this->tasksArray[i];
    }
    file>>this->_cMax;
    file.close();
}
void printArrayOfVector(std::vector<uint32_t>* processorsArray,uint32_t arraySize)
{
    for (uint32_t processorIdx = 0; processorIdx < arraySize; processorIdx++)
    {
        std::cout<<"Processor "<<processorIdx<<" : ";
        for (std::vector<uint32_t>::const_iterator i = processorsArray[processorIdx].begin(); i != processorsArray[processorIdx].end(); ++i)
        std::cout << *i << ' ';
        std::cout<<std::endl;

    }
}
void printArray(uint32_t* array, uint32_t arraySize)
{
    std::cout<<"Tablica:"<<std::endl;
    for (uint32_t i=0;i<arraySize;i++)
    {
        std::cout<<array[i]<<' ';
    }
    
}
    