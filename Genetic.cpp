#include "Genetic.h"
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Individual.h"
#include <time.h>
#include <iostream>
#include <numeric>

Genetic::Genetic(ProblemInstance *pi,
                 uint32_t populationSize,
                 uint32_t numOfChildren,
                 uint32_t numOfParents,
                 uint32_t stopValue,
                 float tourneyRatio,
                 float mutationChance,
                 float mutationRatio,
                 float reproductionRatio,
                 Method method,
                 Condition stopCondition)
                : SchedulingAlgorithm(pi)
{
    this->_populationSize = populationSize;
    this->_numOfChildren = numOfChildren;
    this->_numOfParents = numOfParents;
    this->_stopValue = stopValue;
    this->_tourneyRatio = tourneyRatio;
    this->_mutationChance = mutationChance;
    this->_reproductionRatio = reproductionRatio;
    this->_method = method;
    this->_stopCondition = stopCondition;
    this->problem = pi;
    adjustParameters();
    
    Individual::_problem  = this->problem;

    this->_repArr=nullptr;
    this->_population=nullptr;
    this->_tourArr=nullptr;
    this->_tourPopArr=nullptr;

    this->_siblings = new Individual[this->_numOfChildren];
    this->_childGenes = new uint32_t[pi->numOfTasks];

    this->_rngEngine = new std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    this->_rngProcDistribution=new std::uniform_int_distribution<uint32_t>(0,this->problem->numOfProcessors-1);
    this->_mutationRandGen=new std::uniform_real_distribution<float>(0,1);
    this->_repGrpSizeOrd = genOrderArr(this->_repGroupSize);
    this->_repGenOrd = genOrderArr(this->problem->numOfTasks);

    generatePopulation();
    std::cout << "Parametry - konstruktor:" << std::endl;
    printParameters();
    //std::cout << "Populacja poczatkowa - konstruktor:" << std::endl;
    //printIntArray(problem->tasksArray,problem->numOfTasks);
    //printIndividualsArray(_population,_populationSize);
}
void Genetic::setMethod(Method method)
{
    this->_method = method;
    adjustParameters();
}
uint32_t *Genetic::genOrderArr(uint32_t numOfElements)
{
    uint32_t *arr = new uint32_t[numOfElements];
    for (uint32_t i = 0; i < numOfElements; i++)
    {
        arr[i] = i;
    }
    return arr;
}
void Genetic::adjustParameters()
{

    _repGroupSize = ((float)_numOfParents / (float)_numOfChildren) * _reproductionRatio * (float)_populationSize;
    if (this->_method != Rank)
    {
        if (this->_method == Tourney)
        {
            _fightGroupSize = ceil((float)_populationSize / (float)_repGroupSize);
        }
        if (this->_method == Tourney_Rank)
        {
            _tourneyRatio = _reproductionRatio + (1 - _reproductionRatio) * _tourneyRatio;
            _fightGroupSize = ceil((float)_populationSize / ((float)_repGroupSize / (float)_tourneyRatio));
            _tourGroupSize = _populationSize/_fightGroupSize + (_populationSize % _fightGroupSize != 0);
        }
    }
    this->_parentPart = this->problem->numOfTasks / _numOfParents;
    this->_remGenes = this->problem->numOfTasks % _numOfParents;
    this->_numOfMutation = _mutationRatio * problem->numOfTasks;
    if (_numOfMutation == 0) { _numOfMutation = 1; }
    this->_extendedPopulationSize = _populationSize + _repGroupSize;
}
void Genetic::generatePopulation()
{
    delete[] this->_population;
    this->_population = new Individual[this->_extendedPopulationSize];
    uint32_t* genes;
    genes=new uint32_t[this->problem->numOfTasks];
    for ( uint32_t individual = 0; individual < this->_extendedPopulationSize; individual++)
    {
        for (uint32_t gene = 0; gene<this->problem->numOfTasks; gene++)
        {
            genes[gene]=(*this->_rngProcDistribution)(*this->_rngEngine);
        }
        _population[individual].copyGenes(genes);
    }

    delete[] genes;
}
void Genetic::crossover()
{
    //randomize parent selection
    std::shuffle(_repGrpSizeOrd, _repGrpSizeOrd + _repGroupSize, *_rngEngine);
    //randomize genes selection
    std::shuffle(_repGenOrd, _repGenOrd + problem->numOfTasks, *_rngEngine);
    //genes that need to be taken from first parents coused by numOfTasks%numOfParents!=0
    uint32_t remGenes = this->_remGenes;
    //additional offset coused by numOfTasks%numOfParents!=0
    uint32_t addOffset = 0; 
    uint32_t curParent = 0;
    for (int curParrents = 0; curParrents < _repGroupSize / _numOfParents; curParrents++)
    {
        /*std::cout << "RepArray - crossovers:" << std::endl;
        printIndividualsArray(_repArr, _repGroupSize);
        std::cout << "RepGrpSizeOrd - crossovers:" << std::endl;
        printIntArray(_repGrpSizeOrd, _repGroupSize);
        std::cout << "RepGenOrd - crossovers:" << std::endl;
        printIntArray(_repGenOrd, problem->numOfTasks);*/

        //generate siblings
        for (uint32_t sibling = 0; sibling < _numOfChildren; sibling++)
        {
            //randomize genes selection
            //std::shuffle(_repGenOrd, _repGenOrd + problem->numOfTasks, *_rngEngine);
            //take part of genes from every parent
            for (uint32_t parent = 0; parent < _numOfParents; parent++)
            {
                if (sibling == 0) {
                    curParent = _repGrpSizeOrd[curParrents * _numOfParents + parent];
                }
                else {
                    curParent = _repGrpSizeOrd[curParrents * _numOfParents + 1 - parent];
                }

                //curParent = _repGrpSizeOrd[curParrents * _numOfParents + parent];

                //std::cout << "RepArray[obecny] - crossovers:" << std::endl;
                //printIntArray(_repArr[curParent].genes, problem->numOfTasks);
                //take one more gene if needed from a few first parents
                if (remGenes)
                {
                    _siblings[sibling].copyGenes(_repArr[curParent],
                                               _parentPart + 1,
                                               _repGenOrd,
                                               _parentPart * parent + addOffset);
                    remGenes--;
                    addOffset++;
                }
                else
                {
                    _siblings[sibling].copyGenes(_repArr[curParent],
                                               _parentPart,
                                               _repGenOrd,
                                               _parentPart * parent + addOffset);
                }
            }
            remGenes = this->_remGenes;
            addOffset = 0;

            _siblings[sibling].findFitness();
        }

        uint32_t startIdx= _extendedPopulationSize -(curParrents+1)*_numOfChildren;
        uint32_t finalIdx=startIdx+_numOfChildren;

        //std::cout << "Dzieci - " << curParrents << ":" << std::endl;
        //printIndividualsArray(_siblings, _numOfChildren);

        //Raplace last postion from sorted _population by new individuals
        for (uint32_t idx=startIdx; idx < finalIdx;idx++ )
        {
            mutation(_siblings[idx-startIdx].genes);
            _population[idx]=_siblings[idx-startIdx];
        }

        //std::cout << "Zmutowane dzieci - " << curParrents << ":" << std::endl;
        //printIndividualsArray(_siblings, _numOfChildren);

        //std::cout << "Populacja po dodaniu dzieci:" << std::endl;
        //printIndividualsArray(_population, _populationSize);
        
    }
}

void Genetic::sortPopulation()
{
    std::sort(_population, _population + _extendedPopulationSize,
    [](Individual const & a, Individual const & b) -> bool { return a.fitness < b.fitness; }
    );
}

void Genetic::sortTourArr()
{
    std::sort(_tourArr, _tourArr + _tourGroupSize,
    [](Individual const & a, Individual const & b) -> bool { return a.fitness < b.fitness; }
    );
}

void Genetic::solve()
{
    // Generate arrays
    this->_repArr=new Individual[this->_repGroupSize];
    if(_method == Tourney_Rank)
    {
        this->_tourArr = new Individual[_tourGroupSize];
    }

    // Generate array to fight
    _tourPopArr = genOrderArr(_populationSize);

    // Main
    switch (_stopCondition)
    {
    case Time: {

        const std::chrono::time_point start = std::chrono::high_resolution_clock::now();
        const std::chrono::time_point end = start + std::chrono::seconds(_stopValue);

        while(std::chrono::high_resolution_clock::now() <= end) {
            sortPopulation();
            std::cout << "Najlepszy cMAX wynosi: " << _population[0].fitness << ", cMAX AVG = ";
            std::cout << std::accumulate(_population, _population + _populationSize, 0,
                [](uint32_t a, Individual const& b) -> uint32_t { return b.fitness + a; }) / _populationSize << std::endl;
            solveIterated();
        }
        break;
    }
    case Iteration: {
        for (uint32_t i = 0; i <= _stopValue; i++){
            //std::cout << "Populacja przed sortem w " << i << ":" <<std::endl;
            //printIndividualsArray(_population, _populationSize);
            sortPopulation();
            std::cout << "Najlepszy cMAX wynosi w " << i << " wynosi: "<< _population[0].fitness << " cMAX AVG = ";
            std::cout << std::accumulate(_population, _population + _populationSize, 0, 
                [](uint32_t a, Individual const& b) -> uint32_t { return b.fitness + a; })/_populationSize << std::endl;
            //std::cout << "Populacja po sortem w " << i << ":" << std::endl;
            //printIndividualsArray(_population, _populationSize);
            solveIterated();
        }
        break;
    }
    }
    sortPopulation();
    std::cout << "Najlepszy cMAX wynosi: " << _population[0].fitness << std::endl;

    // Delete arrays
    delete[] _repArr;
    delete[] _tourArr;
    delete[] _tourPopArr;

}

void Genetic::solveIterated()
{
    switch (_method)
    {
    case Rank:
        rank();
        break;
    case Tourney:
        tourney();
        break;
    case Tourney_Rank:
        tourney();
        rank();
        break;
    }
    crossover();

    /*if (_method == Tourney || _method == Tourney_Rank){
        tourney();
    }
    if (_method == Rank || _method == Tourney_Rank){
        rank();
    }
    crossover();*/
}

void Genetic::rank()
{
    switch (_method)
    {
    case Rank:{
        std::copy(_population, _population + _repGroupSize, _repArr);
        break;
    }
    case Tourney_Rank: {
        sortTourArr();
        std::copy(_tourArr, _tourArr + _repGroupSize, _repArr);
        //delete[] _tourArr;//_tourArr też zrób raz w kostruktorze i niszcz raz w destruktorze
        break;
    }
    }
}

void Genetic::tourney()
{
    // Generate outcome array
    Individual* tourOutArrName;
    if(_method == Tourney){
        tourOutArrName = _repArr;
    }
    else{
        tourOutArrName = _tourArr;
    }

    // Shuffle array to fight
    //std::mt19937_64* tourneyEngine = new std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::shuffle(_tourPopArr, _tourPopArr + _populationSize, *_rngEngine);

    // Fight
    for (uint32_t i = 0; i < _populationSize; i++) {
        if (i % _fightGroupSize == 0) {
            tourOutArrName[i/_fightGroupSize] = _population[_tourPopArr[i]];
        }
        else {
            if (_population[_tourPopArr[i]].fitness < tourOutArrName[i/_fightGroupSize].fitness) {
                tourOutArrName[i/_fightGroupSize] = _population[_tourPopArr[i]];
            }
        }
    }

    
    //delete tourneyEngine;
}

Genetic::~Genetic()
{
    delete [] _population;
    delete [] _siblings;
    delete [] _childGenes;
    delete _rngEngine;
    delete _rngProcDistribution;
    delete [] _repGrpSizeOrd;
    delete [] _repGenOrd;
}

void Genetic::printIndividualsArray(Individual *tab, uint32_t size)
{
    for(int element = 0; element < size; element++){
        std::cout<<"Genes: [";
        for(int index = 0; index < problem->numOfTasks; index++){
            std::cout<<tab[element].genes[index]<<" ";
        }
        std::cout<<"] cMAX = "<<tab[element].fitness << " Wskaznik = " << tab[element].genes << std::endl;
    }
}
void Genetic::printParameters()
{
    std::cout<<"Parameters: " << std::endl;
    std::cout<<"_populationSize:"<<_populationSize << std::endl;
    std::cout<<"_fightGroupSize: "<<_fightGroupSize << std::endl;
    std::cout<<"_tourGroupSize:"<<_tourGroupSize << std::endl;
    std::cout<<"_repGroupSize:"<<_repGroupSize << std::endl;
}
void Genetic::printIntArray(uint32_t *array, uint32_t size)
{
    std::cout<<"[";
    for (uint32_t i = 0; i < size; i++)
    {
        std::cout<<array[i]<<" ";
    }
    std::cout<<"]"<<std::endl;
}

void Genetic::mutation(uint32_t *tab) {
    if ((*this->_mutationRandGen)(*this->_rngEngine) <= _mutationChance) {
        for (uint32_t gene = 0; gene < _numOfMutation; gene++) {
            tab[_repGenOrd[gene]] = (*this->_rngProcDistribution)(*this->_rngEngine);
        }
    }
}