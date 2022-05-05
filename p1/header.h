#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#define ARRLEN 25000
#define INTERVAL 1000
#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

/* Author: Jörg Quick, Sebastian Zill */

typedef struct {unsigned long size, resident, share, text, lib, data, dt;} statm_t; //typedef struct für statm_t

unsigned long read_off_memory_status(statm_t& result)
{
    unsigned long dummy;
    const char* statm_path = "/proc/self/statm"; //Pfad zur statm file

    FILE *f = fopen(statm_path, "r"); //fstream zum öffnen des files (read)
    if (!f)
    {
        perror(statm_path);
        abort();
    }
    if (7 != fscanf(f, "%ld %ld %ld %ld %ld %ld %ld", &result.size, &result.resident, &result.share, &result.text, &result.lib, &result.data, &result.dt)) //Schreibt daten in die values
    {
        perror(statm_path);
        abort();
    }
    fclose(f); //Schließt das file
    return result.size;
}

void print_to_file(std::stringstream &str) //Print to file Methode
{
    const char* file_path = "output.txt"; //File path
    std::fstream myFile;
    myFile.open(file_path, std::ios::out);

    if (!myFile) //Wenn Fehler geworfen
    {
        std::cout << "File could not be created!" << std::endl;
        return;
    }
    myFile << str.str(); //Schreibe in File
    myFile.close(); //Schließt das File
}

void printMem(struct rlimit &limit, struct rusage &usage, statm_t &result, std::stringstream &str) //Printen der gesammelten Werte via struct Übergabe
{
    str << "StackLimit: " << (limit.rlim_cur / 1000) << " KB" << std::endl;
    str << "User CPU time: " << usage.ru_utime.tv_sec << " s, " << usage.ru_utime.tv_usec << " ms" << std::endl;
    str << "System CPU time: " << usage.ru_stime.tv_sec << " s, " << usage.ru_stime.tv_usec << " ms" << std::endl;
    str << "MaxRss: " << usage.ru_maxrss << " KB" << std::endl;
    str << "StackSize: " << read_off_memory_status(result) << " KB" << std::endl; //Ausgabe
    str << "----------------------------" << std::endl;
}

void funcMem(int* arrPtr, std::stringstream &str) //Iterative Funktion
{
    // Init
    struct rlimit limit;
    struct rusage usage;
    statm_t result;

    for (int i = 0; i < ARRLEN; i++)
    {
        arrPtr[i] = i;

        if (i % INTERVAL == 0)
        {
            getrlimit(RLIMIT_STACK, &limit);
            getrusage(RUSAGE_SELF, &usage);

            printMem(limit, usage, result, str);
        }
    }
}

void funcRec(int* arrPtr, std::stringstream &str, int pos = 0) //Rekursive Funktion
{
    // Abbruchgrund für rek.
    if (pos >= ARRLEN) return;

    // Init
    struct rlimit limit;
    struct rusage usage;
    statm_t result;

    if (pos % INTERVAL == 0)
    {
        getrlimit(RLIMIT_STACK, &limit);
        getrusage(RUSAGE_SELF, &usage);

        printMem(limit, usage, result, str);
    }
    funcRec(arrPtr, str, (pos + 1));
}

#endif // HEADER_H_INCLUDED
