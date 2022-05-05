#include "Zusteller.h"

Zusteller::Zusteller(){}                // Constructor
Zusteller::~Zusteller(){}               // Destructor


// called from Logistikzentrum to run for each Zusteller thread continuously
void Zusteller::run_zusteller()
{
    while(true)
    {
        // NOTE: Semaphore to signal storage fullness by LZ

        // if recieved a letter from logistikzentrum
        if(recievedLetter.empfaengerId != -1 && recievedLetter.inhalt != "")
        {
            // check next free space in letterStorage
            for(int i = 0; i < ZUSTELLER_LAGER_CAPACITY; i++)
            {
                // if letter recieved and free spot in storage - write into storage and clear var for next letter
                if(letterStorage[i].empfaengerId == -1)
                {
                    letterStorage[i] = recievedLetter;

                    recievedLetter.empfaengerId = -1;
                    recievedLetter.inhalt = "";
                    break;
                }
            }
        }

        // if capacity of storage reached deliver letters
        if(letterStorage[ZUSTELLER_LAGER_CAPACITY - 1].empfaengerId != -1)
        {
            if (myLogistikZentrum->getDeliverMode() == 1)
                deliver_with_notify();
            else
                deliver();
        }
    }
}

// called when letter capacity is full -> deliver letters to empfaenger
void Zusteller::deliver()
{
    // go through all letters
    for (Logistikzentrum::Letter &letter: letterStorage)
    {
        // simulate packaging and driving and open mutex scope
        usleep((useconds_t) rand() % 1000);
        {
            // save letterbox with mutex / no other process on the letterbox now (e.g. Zusteller)
            std::scoped_lock myMtx(myLogistikZentrum->myMtx);

            // empfaengervec -> id of empfaenger = empfaengerId of letter -> push current letter into that letterbox
            myLogistikZentrum->empfaengerVec.at(letter.empfaengerId)->letterBox.push_back(letter);
        }

        // clear current delivered letter
        letter.empfaengerId = -1;
        letter.inhalt = "";
    }
}

// called when letter capacity is full -> deliver letters with notify
void Zusteller::deliver_with_notify()
{
    // go through all letters
    for (Logistikzentrum::Letter &letter: letterStorage)
    {
        // simulate packaging and driving and open mutex scope
        usleep((useconds_t) rand() % 1000);
        {
            // save letterbox with mutex / no other process on the letterbox now (e.g. Zusteller)
            std::scoped_lock myMtx(myLogistikZentrum->myMtx);

            // empfaengervec -> id of empfaenger = empfaengerId of letter -> push current letter into that letterbox
            myLogistikZentrum->empfaengerVec.at(letter.empfaengerId)->letterBox.push_back(letter);
        }

        // NOTE as well: Semaphore functions usage
        // AUFWECKEN: notify empfaenger in new thread to check briefkasten
        std::thread t(&Empfaenger::run_empfaenger_with_notify,(myLogistikZentrum->empfaengerVec.at(letter.empfaengerId)));
        // letter delivered -> go back to our thread / loop
        t.join();

        // clear current delivered letter
        letter.empfaengerId = -1;
        letter.inhalt = "";
    }
}
