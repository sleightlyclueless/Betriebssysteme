#include "Empfaenger.h"

Empfaenger::Empfaenger(){}                  // Constructor
Empfaenger::~Empfaenger(){}                 // Destructor


// Check letterbox on your own periodically
void Empfaenger::run_empfaenger()
{
    while(true)
    {
        usleep((useconds_t) rand() % 1000); // wait for rand microseconds to check letterbox again

        if(!letterBox.empty())              // checked for delivered letters and found some
            handle_letters(false);
    }
}

// Check letterbox on feedback from Zusteller
void Empfaenger::run_empfaenger_with_notify()
{
    if(!letterBox.empty())                  // checked for delivered letters and found some
        handle_letters(true);
}

// Retrieve letters from letterbox with mutex protection
void Empfaenger::handle_letters(bool notify)
{
    // object myMtx handler => gets myMtx from Logistikzentrum myMtx
    // create mutex in this scope: only one thread allowed in here
    std::scoped_lock myMtx(myLogistikZentrum->myMtx);

    // repeat reading letters until letterbox is empty
    while(true)
    {
        if(!letterBox.empty())
        {
            if (notify)
            {
                std::cout << "With notify:" << std::endl;
                print_letter(*letterBox.begin());
            }

            else
            {
                std::cout << "Without notify:" << std::endl;
                print_letter(*letterBox.begin());
            }

            myLogistikZentrum->lettercounter++;
            letterBox.erase(letterBox.begin());
        }
        else{break;}
    }
}

// Print letter total
void Empfaenger::print_letter(Logistikzentrum::Letter &l)
{
    std::cout << "Recieved at Empfaenger with id: " << l.empfaengerId << std::endl
              << "Content: " << l.inhalt << std::endl
              << "=====================" << std::endl;
}
