#include "Storage.h"
#include "Manager.h"

Storage::Storage()
{
    storage = new std::vector<int>;
    Manager::increaseAmount();
    number = Manager::getAmount();
}

void Storage::fillVector()
{
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
         storage->push_back(i);
         std::cout <<  std::endl << "Storing data(" << i << ") in storage(" << number << ")...";
         printUsages();
    }
}

void Storage::readVector()
{
    for (int j = 0; j < VECTOR_SIZE; j++)
    {
        std::cout <<  std::endl << "Reading data(" << storage->at(j) << ") in storage(" << number << ")...";
        printUsages();
    }
}

uint64_t Storage::getMemUsage()
{
    struct rusage r_usage;
    uint32_t ret = getrusage(RUSAGE_SELF, &r_usage);
    if (ret == 0) return r_usage.ru_maxrss;
    return ERROR_CODE;
}

uint32_t Storage::getCpuUsage()
{
    CPUSnapshot previousSnap;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Need 1000ms delay to read CPU usage. (Htop not!)
    CPUSnapshot curSnap;

    const float ACTIVE_TIME = curSnap.GetActiveTimeTotal() - previousSnap.GetActiveTimeTotal();
    const float IDLE_TIME   = curSnap.GetIdleTimeTotal() - previousSnap.GetIdleTimeTotal();
    const float TOTAL_TIME  = ACTIVE_TIME + IDLE_TIME;
    uint32_t usage = 100.f * ACTIVE_TIME / TOTAL_TIME;
    return usage;
}

void Storage::printUsages()
{   
    const uint64_t memUsage = getMemUsage();
    //const uint32_t cpuUsage = getCpuUsage();

    if (memUsage != ERROR_CODE) std::cout << "Memory usage: " << memUsage << " KB.";
    //if (cpuUsage != ERROR_CODE) std::cout << "CPU usage: " << cpuUsage << "%.";
}