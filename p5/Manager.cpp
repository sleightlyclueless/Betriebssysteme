#include "Manager.h"

uint32_t Manager::amount;

Manager::Manager()
{
    amount = 0;
}

uint32_t Manager::getAmount()
{
    return amount;
}

void Manager::increaseAmount()
{
    amount++;
}