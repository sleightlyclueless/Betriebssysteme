// ================================= INCLUDES =================================
#include <iostream>                                                     // console output
#include <cstring>                                                      // string and belonging functions (strcpy
#include <vector>                                                       // use vector for arguments
#include <sstream>                                                      // work with get lines etc.
                                                                        // linux specific libs
#include <unistd.h>                                                     // (getcwd, fork, chdir)
#include <sys/wait.h>                                                   // (wait for pid)



/* ================================= GLOBAL VARS ================================= */
/**
 * @brief Global Variables
 */
size_t tokenCount = 0;                                                  // length of cmd tokens / parameters
pid_t childPid, lastSyncId;                                             // id of last forked processes
int status;                                                             // statuscode of new forked process
bool sync_process = true;                                               // bool if process executed syncrounously (fg) or asynchrounously (bg)
std::vector<int> async_process_ids;                                     // vector of all processes that are asyncrounous (bg)


/* ================================= BUILD ARGUMENT LIST ================================= */
/**
 * @brief Split a string by spaces and return vector of arguments
 *
 * @param line read commands
 * @param args args vector with commands
 */
void split(const std::string &line, std::vector<std::string> &args)
{
    std::istringstream is(line);
    std::string token;
    while (std::getline(is, token, ' '))
        args.push_back(token);
}

/**
 * @brief Read a line into a cmd array of cmd and following space seperated args
 *
 * @param cmd charArray
 * @param params charArray dif.
 */
void read_command(char *cmd, char **params)
{
    std::string line;
    std::vector<std::string> args;
    std::getline(std::cin, line);
    split(line, args);
    tokenCount = args.size();

    if(args.empty())
        args.emplace_back("");
    else if(args.at(0) == "cd")
        chdir(args.at(1).c_str());                                      // for cd command replace second arg in c_str for it to work correctly

    if (args.size() > 1 && args.at(1) == "&")                           // check for synchronus arg & [e.g.: "firefox & youtube.com"]
    {
        sync_process = false;
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

/* ================================= UTILS ================================= */
/**
 * @brief Print all id vector contents
 *
 * @param myvec vector
 */
void print_vector_contents(std::vector<int> &myvec)
{
    if (myvec.empty())
        return;
    
    for (pid_t currId : myvec)
        std::cout << "ID: " << currId << std::endl;
}

/**
 * @brief Push ids into async_process_ids vector if id does not exist in there
 *
 * @param myvec vector
 */
void push_id_if_not_exists(std::vector<int> &myvec)
{
    for (pid_t currId : myvec)
        if (currId == childPid)
            return;

    myvec.push_back(childPid);
}

/**
 * @brief Remove id if id exists in vector
 *
 * @param myId id to remove
 * @param myvec vector
 */
void remove_id_if_exists(pid_t &myId, std::vector<int> &myvec)
{
    if (myvec.empty())
        return;

    int counter = 0;
    for (pid_t currId : myvec)
    {
        if (currId == myId)
        {
            myvec.erase(myvec.begin() + counter);
            return;
        }
        counter++;
    }
}


/* ================================= HANDLE SIGNALS ================================= */
/**
 * @brief Signal handle function
 *
 * @param sigId as param for signal id
 */
void handle_signal(int sigId)
{
    if (sigId == 20)                                                    // signal for CTRL + Z -> Pause current sync process
    {
        printf("CTRL + Z [%d] called!\n\r", sigId);
        if (lastSyncId == 0)                                            // if no sync process running print error msg
        {
            std::cout << ": No synchronos child to abort exists!" << std::endl;
            return;
        }
        std::cout << "Pausing child with id: " << lastSyncId << std::endl;
        kill(lastSyncId, SIGTSTP);
    }
    else if (sigId == 17)                                               // signal for child process handling
    {
        int mystatus;
        pid_t pid = waitpid(-1, &status, WNOHANG);                      // get pid of exited parent process
        if (0 < pid && WIFEXITED(status))                               // if the child process was closed remove from async vector if exists
            remove_id_if_exists(pid, async_process_ids);
    }
    else
        printf("Signal [%d] called but unknown!\n\r", sigId);           // unknown signal handling
}


/**
 * @brief Main function
 *
 * @return int as closing param
 */
int main() {
    char cmd[20], *params[60];                                          // user input cmd and arguments
    bool exit = false;                                                  // bool wether user wants to quit

    signal(SIGTSTP, handle_signal);                                     // CTRL + Z handler -> Pause current waitForChild process
    signal(SIGCHLD, handle_signal);                                     // child Process handler -> check and process child state

    while (exit == false)
    {
        char cwd[256];
        std::cout << getcwd(cwd, sizeof(cwd)) << "> ";                  // shell optic: Print current path beforehand

        read_command(cmd, params);                                      // read user input into cmd and parameter vector


        if (strcmp(cmd, "logout") == 0 || strcmp(cmd, "exit") == 0)     // check for custom logout prompt to exit shell
        {
            if (!async_process_ids.empty())                             // check if bg processes still running for logout prompt to avoid zombies - restart
            {
                std::cout << "Cant logout! Some synchronous processes with following ids still running:" << std::endl;
                print_vector_contents(async_process_ids);
                continue;
            }
            std::cout << "Quit shell? (y/n)?" << std::endl;             // logout prompt and according settings for loop flag
            char answer;
            std::cin >> answer;

            if (answer != 'y')
                memset(cmd, 0, sizeof(cmd));                            // changed mind: clear cmd
            else
                exit = true;                                            // not changed mind: set exit flag to exit loop
        }

        else if (strcmp(cmd, "stop") == 0)                              // stop cmd -> stop process
        {
            pid_t killme = std::stoi(params[1]);
            std::cout << "Stopping: " << killme << std::endl;
            kill(killme, SIGTSTP);
        }

        else if (strcmp(cmd, "cont") == 0)                              // cont cmd -> continue process
        {
            pid_t killme = std::stoi(params[1]);
            std::cout << "Continueing: " << killme << std::endl;
            if (killme == lastSyncId)                                   // if process to continue was a sync process wait for its status change again
            {
                kill(killme, SIGCONT);
                std::cout << "Waiting for id: " << killme << std::endl;
                waitpid(killme, &status, WUNTRACED);
                std::cout << "Child process continued with status code: " << status << std::endl;
            }
            else
                kill(killme, SIGCONT);

        }

        else                                                            // not custom cmd but system execvp cmds
        {
            childPid = fork();                                          // FORK: Start a new Process as duplicate of current process on other cmds
            setpgid(childPid, childPid);                                // set pid group to be able to kill group processes

            if (childPid < 0)                                           // no child process could be created by fork => return -1
            {
                std::cerr << "Error: Can't fork!" << std::endl;
                return -1;
            }
            else if (childPid == 0)                                     // child process created => return 0
            {
                status = execvp(cmd, params);                           // EXECVP: Execute a file executeable (file-to-execute, argument-array of params for file)
                return status;
            }
            else                                                        // no new child process created => return id of current parent process in childId
            {
                if (sync_process)                                       // if synchronous call: wait for child status change
                {
                    std::cout << "Waiting for id: " << childPid << std::endl;
                    lastSyncId = childPid;
                    waitpid(childPid, &status, WUNTRACED);              // WUNTRACED: Return true if process stopped
                    std::cout << "Child process continued with status code: " << status << std::endl;
                }
                else                                                    // if asynchronous call - save into async vector to check later on logout if all processes by shell closed again
                {
                    push_id_if_not_exists(async_process_ids);
                    if (async_process_ids.size() == 1)                   // print id information on new process
                        std::cout << "Child process with id: " << async_process_ids.at(0) << " called!" << std::endl;
                    else
                        std::cout << "Child process with id: " << async_process_ids.at(async_process_ids.size() -2) << " called!" << std::endl;
                }
            }
        }

        for (int i = 0; i < tokenCount; i++)                             // clear params for next iteration
            delete params[i];
        sync_process = false;
    }
}
