#ifndef ZUSTELLER_H
#define ZUSTELLER_H
#include "Logistikzentrum.h"
#include "Empfaenger.h"

#define ZUSTELLER_LAGER_CAPACITY 5

class Zusteller
{
    public:
        Zusteller();                            // Constructor
        virtual ~Zusteller();                   // Destructor

        // Atrributes
        Logistikzentrum* myLogistikZentrum;     // Zusteller gets reference to LogistikZentrum
        Logistikzentrum::Letter recievedLetter; // Variable for letter construction
        Logistikzentrum::Letter letterStorage[ZUSTELLER_LAGER_CAPACITY];    // Array with letters with a max capacity
        
        // Methods
        void run_zusteller();                   // called from logistikzentrum to run for each zusteller thread continuously
        void deliver();                         // called when letter capacity is full -> deliver letters
        void deliver_with_notify();             // called when letter capacity is full -> deliver letters with notify

};

#endif // ZUSTELLER_H
