#include <iostream>                                                     // console output
#include <cstring>                                                      // string and belonging functions (strcpy
#include <vector>                                                       // use vector for arguments
#include <sstream>                                                      // work with get lines etc.
                                                                        // linux specific libs
#include <unistd.h>                                                     // (getcwd, fork, chdir)
#include <sys/wait.h>                                                   // (wait for pid)


size_t tokenCount = 0;                                                  // public tokencount used across functions
bool waitForChild = false;                                              // public waitForChild wether we wait for forked child process across functions

void split(const std::string &line, std::vector<std::string> &args)     // split a string by spaces and return vector of arguments
{
    std::istringstream is(line);
    std::string token;
    while (std::getline(is, token, ' '))
        args.push_back(token);
}

void read_command(char *cmd, char **params)                             // read a line into a cmd array of cmd and following space seperated args
{
    std::string line;                                                   // get and split line into vector of args and get their length
    std::vector<std::string> args;
    std::getline(std::cin, line);
    split(line, args);
    tokenCount = args.size();

    if(args.empty())
        args.emplace_back("");
    else if(args.at(0) == "cd")
        chdir(args.at(1).c_str());                                      // for cd command replace second arg in c_str for it to work correctly

    if (args.size() > 1 && args.at(1) == "&")                           // check for asynchronus arg &
    {
        waitForChild = true;
        args.erase(std::next(args.begin()));                            // erase the & from argument list
    }
    strcpy(cmd, args.at(0).c_str());                                    // copy the cmd into the cmd chararr

    for (int i = 0; i < args.size(); i++)                               // copy other arguments into parameters vector
    {
        params[i] = new char[128];
        strcpy(params[i], args.at(i).c_str());
    }
    params[args.size()] = nullptr;
}

int main() {
    char cmd[20], *params[60];                                          // user input cmd and arguments
    pid_t childPid;                                                     // id of forked process
    int status;                                                         // statuscode of new forked process
    std::vector<int> pids;                                              // vector with forked pids
    bool exit = false;                                                  // bool wether user wants to quit

    while (exit == false)
    {
        if (!pids.empty())                                              // print all pids of prev child processes
        {
            std::cout << std::endl;
            for (int &pid : pids)
                std::cout << pid << std::endl;
        }

        char cwd[256];
        std::cout << getcwd(cwd, sizeof(cwd)) << "> ";                  // shell optic: Print current path beforehand

        read_command(cmd, params);                                      // read user input into cmd and parameter vector

        if (strcmp(cmd, "logout") == 0 || strcmp(cmd, "exit") == 0)     // check for custom logout prompt to exit shell
        {
            std::cout << "Quit shell? (y/n)" << std::endl;
            char answer;
            std::cin >> answer;

            if (answer != 'y')
                memset(cmd, 0, sizeof(cmd));                            // changed mind: Clear cmd
            else
                exit = true;                                            // not changed mind: set exit flag to exit loop

            continue;                                                   // handled logout cmd - start atop or exit loop on exit = true
        }

        childPid = fork();                                              // FORK: Start a new Process as duplicate of current process on other cmds

        if (childPid < 0)                                               // wenn kein child-prozess erzeugt werden konnte -> Rückgabewert = -1
        {
            std::cerr << "Error: Can't fork" << std::endl;
            return -1;
        }
        else if (childPid == 0)                                         // wenn child-prozess erzeugt werden konnte -> Rückgabewert = 0
        {
            status = execvp(cmd, params);                               // EXECVP: Execute a file executeable (file-to-execute, argument-array of params for file)
            return status;
        }
        else                                                            // wenn noch im eltern-prozess
        {
            if (waitForChild)                                           // wenn asynchron auf child warten soll
            {
               waitpid(childPid, &status, WUNTRACED | WCONTINUED);
               std::cout << "Child process finished with status code " << status << std::endl;
               pids.push_back(childPid);                                // save parent id into vector when we are waiting for background child
            }
        }

        for (int i = 0; i < tokenCount; i++)                            // clear params for next iteration
            delete params[i];
        waitForChild = false;
    }
}
