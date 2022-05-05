#ifndef LOGISTIKZENTRUM_H
#define LOGISTIKZENTRUM_H
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <unistd.h>
#include <random>
#include <mutex>
#include <chrono>

// Cross reference to lower classes / threads Empfaenger + Zusteller
// Forward declaration: First generate classes Empfaenger and Zusteller before working on Logistikzentrum
class Empfaenger;
class Zusteller;

class Logistikzentrum
{
    public:
        Logistikzentrum(int &zusteller, int &empfaenger, int &letters, int &mode); // Constructor
        virtual ~Logistikzentrum();             // Destructor

        // Global structs
        struct Letter                           // Struct for Letter (empfaengerId and content)
        {
            int empfaengerId = -1;
            std::string inhalt = "";
        };

        // Public attributes
        std::vector<Empfaenger*> empfaengerVec; // Empfaenger objects (also used by Zusteller to deliver to those ids)
        int lettercounter = 0;                  // Amount of letters recieved though all in total

        // Global mutex instance as freeze protection layer: Everytime Briefkasten is involved
        // called everytime in critical spots of code - where possible mutliple instances access same object to write
        std::mutex myMtx;

        // Methods
        Letter brief_init();                    // Generate a Letter with according contents
        void run_logistikzentrum();             // Initially run Logistikzentrum continuously
        int getLetterPerSecond() const { return r / 1000; }
        int getDeliverMode() const { return d; }// Get value of delivermode (notify or not)


    private:
        // Attributes                           // Note: Amount of Logistikzentrum = 1
        int m = -1;                             // M: Amount of Zusteller
        int n = -1;                             // N: Amount of Empfänger
        int r = -1;                             // R: Time waited between letter generation
        int d = -1;                             // D: Deliver without (0) or with (1) notify

        std::vector<Zusteller*> zustellerVec;   // Zusteller objects

};

#endif // LOGISTIKZENTRUM_H
