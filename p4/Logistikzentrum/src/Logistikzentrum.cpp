#include "main.h"
#include "Logistikzentrum.h"
#include "Empfaenger.h"
#include "Zusteller.h"

// Constructor
Logistikzentrum::Logistikzentrum(int &zusteller, int &empfaenger, int &letters, int &mode)
{
    m = zusteller;
    n = empfaenger;
    r = letters;
    d = mode;

    // generate m Zusteller
    for(int i = 0; i < m; i++)
    {
        Zusteller* z = new Zusteller;
        z->myLogistikZentrum = this;
        zustellerVec.push_back(z);
    }

    // generate n Empfaenger
    for(int i = 0; i < n; i++)
    {
        Empfaenger* e = new Empfaenger;
        e->myLogistikZentrum = this;
        empfaengerVec.push_back(e);
    }

}

// Destructor
Logistikzentrum::~Logistikzentrum(){}


// generate a Letter with according contents
Logistikzentrum::Letter Logistikzentrum::brief_init()
{
    // generate a new letter with designated Empfaenger
    Letter b;
    b.empfaengerId = rand() % n;
    b.inhalt = letterContents.at(rand()%10);
    return b;
}

// initial call from main to this function - continuous loop and simulation
void Logistikzentrum::run_logistikzentrum()
{
    std::vector<std::thread> zustellerThreads;
    std::vector<std::thread> empfaengerThreads;

    // ini continuous Zusteller threads
    for(int i = 0; i < m; i++)
        zustellerThreads.emplace_back(&Zusteller::run_zusteller, zustellerVec.at(i));

    // ini continuous Empfaenger threads if notify mode is turned off start Empfaenger threads that continuously look for letters
    if (d == 0)
        for(int i = 0; i < n; i++)
            empfaengerThreads.emplace_back(&Empfaenger::run_empfaenger, empfaengerVec.at(i));

    // get starttime to calculate rate
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // continuous loop
    while(true)
    {
        // current runtime for elapsed calculation
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
        double calcElapsed = (double)lettercounter / (double)elapsed;
        int calcElapsedInt = calcElapsed * 100;
        calcElapsed = (double)calcElapsedInt / 100;

        if (calcElapsed > 0)
            std::cout << "Rate of letters per second: " << calcElapsed << std::endl;


        // amount of letters generated per second - wait till next one
        Letter b = brief_init();
        usleep((useconds_t) r);


        while(1)
        {
            // choose random zusteller
            int i = rand() % m;

             // if zusteller available to get new letter - give him that one to process
            if(zustellerVec.at(i)->recievedLetter.empfaengerId == -1)
            {
                zustellerVec.at(i)->recievedLetter = b;
                break;
            }
        }
    }



    // close threads when closed
    // wait for Zusteller threads
    for(int i = 0; i < m; i++)
    {
        zustellerThreads.at(0).join();
        zustellerThreads.erase(zustellerThreads.begin());
    }

    // wait for empfaenger threads if notify mode is turned off
    if (d == 0)
    {
        for(int i = 0; i < n; i++)
        {
            empfaengerThreads.at(0).join();
            empfaengerThreads.erase(empfaengerThreads.begin());
        }
    }
}
