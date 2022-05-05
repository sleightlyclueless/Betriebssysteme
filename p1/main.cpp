#include <iostream>
#include "header.h"

/* Author: Jörg Quick, Sebastian Zill */

int main()
{
    std::stringstream str;
    str << "ArraySize: " << ARRLEN << std::endl;
    str << "Interval: " << INTERVAL << std::endl;
    str << "----------------------------" << std::endl;

    int array[ARRLEN];
    funcMem(array, str);
    funcRec(array, str);

    std::cout << str.str();
    print_to_file(str);
    return 0;
}
