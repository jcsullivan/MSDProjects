//
//  main.cpp
//  UnixShell
//
//  Created by Jonathan Sullivan on 2/8/20.
//  Copyright © 2020 Jonathan Sullivan. All rights reserved.
//
//  *****
//  Strongly recommend compiling with: g++ -std=c++17 -o main main.cpp shelpers.cpp -lreadline
//  In order to support readline functionality.
//  *****

#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "shelpers.hpp"
using namespace std;

// Set to "true" for additional printf checkpoints.
#define DEBUGGING false

// Helper function to check on background processes.
void backgroundCheck()
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid < 0 && errno != ECHILD)
    {
        perror("Error with background process.");
    }
    else if (pid > 0)
    {
        cout << "Background process " << pid << " completed with status " << status << "\n";
    }
}

int main(int argc, const char * argv[]) {
    const int READ = 0;
    const int WRITE = 1;
    
    cout << "Hello world.\n";
    
    while(1)
    {
        // The below block is the tab-completion / command history functionality.
        cout << "MSDTerminal";
        char *buffer = readline("> ");
        if (strlen(buffer) > 0)
        {
            add_history(buffer);
        }
        
        string stringifiedBuffer = string(buffer);
        vector<string> tokens = tokenize(stringifiedBuffer);
        
        // Handles if someone hits enter on a blank line.  
        if(tokens.size() == 0)
        {
            continue;
        }
        
        // Allows for displaying variable values by typing $[varname].
        if (tokens[0] == "$")
        {
            char* variable = getenv(tokens[1].c_str());
            if (variable != NULL)
            {
                cout << variable << "\n";
            }
            else
            {
                cout << "No variable of name " << tokens[1].c_str() << ".\n";
            }
            continue;
        }
        
        // Allows for assigning variable values.
        if (tokens[1] == "=")
        {
            const char* varName = tokens[0] .c_str();
            const char* varValue = tokens[2].c_str();
            int setResult = setenv(varName, varValue, 0);
            if (setResult < 0)
            {
                perror("** Error setting environment variable.");
            }
            else
            {
                if(DEBUGGING) {cout << "** Set environment variable: " << varName << " to " << varValue << "\n";}
            }
            continue;
        }
        
        // Checks for completed background processes after every command.
        backgroundCheck();
        
        // Gets vector of commands.
        vector<Command> newCommandVect = getCommands(tokens);
        
        // For each command in the vector, forks and runs the command in the child.
        for (int i = 0; i < newCommandVect.size(); i++)
        {
            // Handles changing directories.
            if(newCommandVect[i].exec == "cd")
            {
                if (DEBUGGING) {cout << "** Start cd. \n";}
                
                const char* pathname;
                if (newCommandVect[i].argv.size() == 2)
                {
                    // Necessary for argument-less cd command.
                    pathname = getenv("HOME");
                }
                else
                {
                    pathname = newCommandVect[i].argv[1];
                }
                if (DEBUGGING) {cout << "** Changing directory: ";
                    cout << pathname;
                    cout << "\n";
                }
                int cdResult = chdir(pathname);
                if (cdResult < 0)
                {
                    perror("** Error changing directories.");
                }
                continue;
            }
            
            // Handles exit specifically.
            if(newCommandVect[i].exec == "exit")
            {
                cout << "Goodbye world.\n";
                exit(0);
            }
            
            pid_t resultPid = fork();
            
            if (resultPid == 0) // Child.
            {
                if (DEBUGGING)
                {
                    operator<<(cout, newCommandVect[i]);
                    printf("\n");
                }

                // If a command comes in with a non-standard stdin, update things accordingly.
                if (newCommandVect[i].fdStdin != READ)
                {
                    if (DEBUGGING) {cout << "** Changing in.\n";}
                    int dupResult = dup2(newCommandVect[i].fdStdin, READ);
                    if (dupResult < 0)
                    {
                        perror("** Error dup-ing stdin.");
                        close(READ);
                        exit(1);
                    }
                    close(newCommandVect[i].fdStdin);
                    if (DEBUGGING) {dprintf(WRITE, "** In duping complete.\n");}
                }
                // If a command comes in with a non-standard stdout, update things accordingly.
                if (newCommandVect[i].fdStdout != WRITE)
                {
                    if (DEBUGGING) {cout << "** Changing out.\n";}
                    int dupResult = dup2(newCommandVect[i].fdStdout, WRITE);
                    if (DEBUGGING) {dprintf(WRITE, "** New out: %d\n", dupResult);}
                    if (dupResult < 0)
                    {
                        perror("** Error dup-ing stdout.\n");
                        close(WRITE);
                        exit(1);
                    }
                    close(newCommandVect[i].fdStdout);
                    if (DEBUGGING) {dprintf(WRITE, "** Out duping complete.\n");}
                }
                const char* commandExec = newCommandVect[i].exec.c_str();
                const char** tempArgv = newCommandVect[i].argv.data();
                char* const* commandArgv = const_cast <char**>(tempArgv);
                if (DEBUGGING) {dprintf(WRITE, "** Execution starting.\n");}
                int executionResult = execvp(commandExec, commandArgv);

                // This code will not be reached unless execvp suffers some fatal error.
                if (executionResult < 0)
                {
                    dprintf(1, "** Error executing commands.\n");
                    close(newCommandVect[i].fdStdin);
                    close(newCommandVect[i].fdStdout);
                    exit(1);
                }

                if (DEBUGGING) {dprintf(WRITE, "** Execution complete.\n");}
                
                exit(0);
            }
            
            if (resultPid > 0) // Parent.
            {
                if (newCommandVect[i].background)
                {
                    // Parent does nothing if process is backgrounded, except close pipes later.
                }
                else
                {
                    // Otherwise parent waits for child to terminate, and reports on its efficacy.
                    int returnStatus;
                    waitpid(resultPid, &returnStatus, 0);
                    
                    if (returnStatus == 0 && DEBUGGING)
                    {
                        printf("** Child terminated appropriately.\n");
                    }
                    if (returnStatus > 0)
                    {
                        perror("** Child terminated with an error.\n");
                    }
                }
                
                // Close non-standard stdin and stdouts.  DO NOT CLOSE standard stdin (0), or terminal will close.
                if (newCommandVect[i].fdStdin != READ)
                {
                    close(newCommandVect[i].fdStdin);
                }
                if (newCommandVect[i].fdStdout != WRITE)
                {
                    close(newCommandVect[i].fdStdout);
                }
                continue;
            }
    
            if (resultPid < 0) // Error.
            {
                // In the case of an error, close all commands inputs and outputs.  
                perror("** Forking error.");
                close(newCommandVect[i].fdStdin);
                close(newCommandVect[i].fdStdout);
                exit(1);
            }
        }
    }
    return 0;
}
