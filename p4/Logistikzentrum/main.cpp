#include <iostream>
#include "Logistikzentrum.h"

int main()
{
    int zusteller, empfaenger, letters, mode;
    std::cout << "Welcome to the logistics - simulation!" << std::endl << std::endl;

    std::cout << "How many Zusteller? ";
    std::cin >> zusteller;
    std::cout << zusteller << " Zustellers set!" << std::endl;

    std::cout << "How many Empfaenger? ";
    std::cin >> empfaenger;
    std::cout << empfaenger << " Empfaenger set!" << std::endl;

    std::cout << "How much time waited between letter generation (in milliseconds)? ";
    std::cin >> letters;
    letters = letters * 1000;
    std::cout << letters << " Zustellers set!" << std::endl;

    std::cout << "Should Empfaenger be notified? " << std::endl
                << "0: No" << std::endl
                << "1: Yes" << std::endl;

    std::cin >> mode;
    std::cout << "Mode set to "<< mode << std::endl;

    // Create new Logistikzentrum on thread and wait for that thread
    Logistikzentrum l = Logistikzentrum(zusteller, empfaenger, letters, mode);
    std::thread mainThread(&Logistikzentrum::run_logistikzentrum, &l);

    // Wait for thread to not reach return 0
    mainThread.join();

    return 0;
}
