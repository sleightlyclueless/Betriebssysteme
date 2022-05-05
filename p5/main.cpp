#include <iostream>
#include "Storage.h"
#include "Manager.h"

int main()
{
    /* Init manager */
    Manager *manager = new Manager();

    Storage *s1 = new Storage();
    Storage *s2 = new Storage();
    /* Fill Vector */
    s1->fillVector();
    s2->fillVector();
    
    /* Read Vector */
    s1->readVector();
    s2->readVector();
    /* Close programm */
    return 0;
}