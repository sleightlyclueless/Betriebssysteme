#ifndef EMPFAENGER_H
#define EMPFAENGER_H
#include "Logistikzentrum.h"

class Empfaenger
{
    public:
        Empfaenger();                                   // Constructor
        virtual ~Empfaenger();                          // Destructor

        // Attributes
        int id;                                         // Empfaenger id
        Logistikzentrum* myLogistikZentrum;             // Empfaenger gets pointer to LZ
        std::vector<Logistikzentrum::Letter> letterBox; // Vector of letters in letterbox

        // Methods
        void run_empfaenger();                          // Check letterbox on your own periodically
        void run_empfaenger_with_notify();              // Check letterbox on feedback from Zusteller
        void handle_letters(bool notify);               // Retrieve letters from letterbox with mutex protection
        void print_letter(Logistikzentrum::Letter &l);  // Print letter contents
};

#endif // EMPFAENGER_H
