#include <iostream>
#include <sys/resource.h>
#include <vector>
#include <chrono>
#include <thread>
#include "CPUSnapshot.h"

//=================================================
#define VECTOR_SIZE 10000000
#define ERROR_CODE -1
//=================================================

class Storage
{
public:
    Storage();

    void fillVector();
    void readVector();

private:
    std::vector<int>* storage;
    uint32_t number;

    uint64_t getMemUsage();
    uint32_t getCpuUsage();

    void printUsages();
};