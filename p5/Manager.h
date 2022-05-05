#include <iostream>
#include <vector>

class Manager
{
public:
    Manager();

    static uint32_t getAmount();
    static void increaseAmount();

private:
    static uint32_t amount;

};
