#include "Genetic.h"
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Individual.h"
#include <time.h>
#include <iostream>
#include <numeric>
#include <cmath>

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
    this->_mutationRatio = mutationRatio;
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
    this->_rngCrossCut = new std::uniform_int_distribution<uint32_t>(1, problem->numOfTasks-1);
    this->_mutationRandGen=new std::uniform_real_distribution<float>(0,1);
    this->_repGrpSizeOrd = genOrderArr(this->_repGroupSize);
    this->_repGenOrd = genOrderArr(this->problem->numOfTasks);
    this->_crossOrder = genOrderArr(this->problem->numOfTasks);

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
    this->_numOfMutation = this->_mutationRatio * (float)this->problem->numOfTasks;
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

void Genetic::crossover2(){
    

    uint32_t maxFitNum0 = 0;
    for (uint32_t i = 0; i < ceil(_repGroupSize * 0.95); i++) {
        maxFitNum0 += std::pow(_population[_populationSize - 1].fitness - _population[i].fitness, 2) + 1;
    }

    uint32_t maxFitNum1 = 0;
    for(uint32_t i = 0; i < floor(_repGroupSize*0.95); i++){
        maxFitNum1+=std::pow(_population[_populationSize-1].fitness - _population[i].fitness,2) + 1;
    }

    std::uniform_int_distribution<uint32_t>* random0 = new std::uniform_int_distribution<uint32_t>(0, maxFitNum0 - 1);
    std::uniform_int_distribution<uint32_t>* random1 = new std::uniform_int_distribution<uint32_t>(0, maxFitNum1 - 1);

    for(uint32_t child = 0; child < _repGroupSize; child+=2){
        uint32_t index1 = index(random0, 0);
        uint32_t index2 = 0;
        do {
            index2 = index(random1, 1);
        } while(index2 == index1);
        //std::cout << "Index1 = " << index1 << " Index2 = " << index2 << std::endl;

        // Lista porządkowa
        std::shuffle(_repGenOrd, _repGenOrd + problem->numOfTasks, *_rngEngine);

        // Miejsce przecięcia
        uint32_t cutPlace = (*this->_rngCrossCut)(*this->_rngEngine);

        _population[_populationSize+child]=_repArr[index1];
        _population[_populationSize+child].copyGenes(_population[index2], cutPlace, _crossOrder, 0);
        mutation(_population[_populationSize+child].genes);
        _population[_populationSize+child].findFitness();

        _population[_populationSize+child+1]=_repArr[index2];
        _population[_populationSize+child+1].copyGenes(_population[index1], cutPlace, _crossOrder, 0);
        //mutation(_population[_populationSize+child+1].genes);
        mutation2(_population[_populationSize + child + 1].genes);
        _population[_populationSize+child+1].findFitness();

        /*_population[_populationSize+child]=_repArr[index1];
        _population[_populationSize+child].copyGenes(_population[index2], problem->numOfTasks/2, _repGenOrd, 0);
        mutation(_population[_populationSize+child].genes);
        _population[_populationSize+child].findFitness();

        _population[_populationSize+child+1]=_repArr[index2];
        _population[_populationSize+child+1].copyGenes(_population[index1], problem->numOfTasks/2, _repGenOrd, 0);
        //mutation(_population[_populationSize+child+1].genes);
        mutation2(_population[_populationSize + child + 1].genes);
        _population[_populationSize+child+1].findFitness();*/
    }

    delete random0;
    delete random1;
}

uint32_t Genetic::index(std::uniform_int_distribution<uint32_t>* ran, uint32_t idx){
    uint32_t ranNum = (*ran)(*this->_rngEngine);

    uint32_t maxZakres = 0;
    if (idx == 0) { maxZakres = ceil(_repGroupSize * 0.95); }
    else { maxZakres = floor(_repGroupSize * 0.95); }

    uint32_t curPos = 0;
    for (uint32_t i = 0; i < maxZakres; i++) {
        if (ranNum >= (curPos + std::pow(_population[_populationSize - 1].fitness - _population[i].fitness, 2) + 1)) {
            curPos += (std::pow(_population[_populationSize - 1].fitness - _population[i].fitness, 2) + 1);
        }
        else {
            return i;
        }
    }
    return maxZakres - 1;
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
                [](uint32_t a, Individual const& b) -> uint32_t { return b.fitness + a; }) / _populationSize /*<< std::endl*/;
            std::cout << " Najgorszy cMAX = " << _population[_populationSize - 1].fitness << std::endl;
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
                [](uint32_t a, Individual const& b) -> uint32_t { return b.fitness + a; })/_populationSize/* << std::endl*/;
            std::cout << " Najgorszy cMAX = " << _population[_populationSize-1].fitness << std::endl;
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
    crossover2();

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

void Genetic::mutation2(uint32_t *tab) {
    if ((*this->_mutationRandGen)(*this->_rngEngine) <= _mutationChance) {
        /*for (uint32_t gene = 0; gene < _numOfMutation; gene++) {
            tab[_repGenOrd[gene]] = (*this->_rngProcDistribution)(*this->_rngEngine);
        }*/
        std::uniform_int_distribution<uint32_t>* numMut = new std::uniform_int_distribution<uint32_t>(1, _numOfMutation);
        uint32_t maxMutNum = (*numMut)(*this->_rngEngine);
        for (uint32_t gene = 0; gene < maxMutNum; gene++) {
            tab[_repGenOrd[gene]] = (*this->_rngProcDistribution)(*this->_rngEngine);
        }
        delete numMut;
    }
}

void Genetic::mutation(uint32_t* tab) {
    if ((*this->_mutationRandGen)(*this->_rngEngine) <= _mutationChance) {
        //for (uint32_t gene = 0; gene < _numOfMutation; gene++) {
        std::uniform_int_distribution<uint32_t>* numMut = new std::uniform_int_distribution<uint32_t>(1, _numOfMutation);
        uint32_t maxMutNum = (*numMut)(*this->_rngEngine);
        for (uint32_t gene = 0; gene < maxMutNum; gene++) {
            if (gene == (_numOfMutation - 1)) {
                uint32_t tmp = tab[_repGenOrd[gene]];
                tab[_repGenOrd[gene]] = tab[_repGenOrd[0]];
                tab[_repGenOrd[0]] = tmp;
            }
            else {
                uint32_t tmp = tab[_repGenOrd[gene]];
                tab[_repGenOrd[gene]] = tab[_repGenOrd[gene + 1]];
                tab[_repGenOrd[gene + 1]] = tmp;
            }
        }
        delete numMut;
    }
}

Genetic::~Genetic()
{
    delete [] _population;
    delete [] _siblings;
    delete [] _childGenes;
    delete _rngEngine;
    delete _rngProcDistribution;
    delete _rngCrossCut;
    delete [] _repGrpSizeOrd;
    delete [] _repGenOrd;
    delete [] _crossOrder;
}