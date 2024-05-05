#include "../header_files/internal_commands.h" // String, Vector
#include "../header_files/util.h" // String, Vector, Unistd.h
#include "../header_files/history_manager.h" // HistoryManager
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/wait.h>

// Example input >>>>> ("" are just for representation)     "source filename.sh [args...]"
// This is the same as though the user inputted this command: ". filename.sh [args...]"
// '.' and SOURCE are similar commands, if you need to research what 'source' does.

/*
@Author Theint Nwe Nyein
Date 4/15/24
Log: Updated the program to take additional arguments  
*/

/*
@author Theint Nwe Nyein
@date 4/19/2024
@log: Added executeInternalCommand function, but was not able to test it yet
*/

// This function is used to execute shell commands in the child process

int executeShellCommand(const std::string& filename, const std::vector<std::string>& args) {
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        return -1;

    } else if (pid == 0) {
        // Child process
        std::vector<const char*> argv;
        argv.reserve(args.size() + 3);
        argv.push_back("bash");
        argv.push_back(filename.c_str());

        for (const auto& arg : args) {
            argv.push_back(arg.c_str());
        }
        argv.push_back(nullptr);
        execvp("bash", const_cast<char* const*>(argv.data())); // Returns only if there is an error
        std::cerr << "Error executing command: " << filename << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            std::cerr << "Child process did not terminate normally" << std::endl;
            return -1;
        }
    }
}

void executeInternalCommand(HistoryManager& hm, std::string command, std::vector<std::string> args){
    if (command == "cd"){
        internal_cd(hm, args[0]);
    }
    else if(command == "alias"){
        internal_alias(args[0]);
    }
    else if(command == "unalias"){
        internal_unalias(args[0]);
    }
    else if(command == "help"){
        internal_help(args[0]);
    }
    else if(command == "set"){
        internal_set(args[0]);
    }
    /*
    else if(command == "shift"){
        internal_shift(args[0], num);
    }
    */
   
    else{
        std::cerr << "Command not found" << std::endl;
    }
}

// Function to parse the input command line
std::pair<std::string, std::vector<std::string>> parseCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string commandName;
    iss >> commandName; // Get the "source" keyword
    std::string filename;
    iss >> filename; // Separate the filename from the arguments
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg); // Push back all the provided args
    }
    return std::make_pair(filename, args);
}


bool internal_source(HistoryManager& hm, std::string command) {
    std::pair<std::string, std::vector<std::string>> parsedCommand = parseCommand(command);
    std::string filename = parsedCommand.first;
    std::vector<std::string> args = parsedCommand.second;
    if (filename == "cd" || filename == "alias" || filename == "unalias" ||
        filename == "help" || filename == "set" || filename == "shift") {
        executeInternalCommand(hm,filename, args);
    } else {
        executeShellCommand(filename, args);
    }
    return true;
}

//Testing the source command
/*
int main(){
    std::string command = "source testing_script.sh arg1 arg2"; 
   bool success = internal_source(command);
    if (!success) {
        std::cerr << "Failed to execute source command" << std::endl;
        return 1;
    }
    return 0;
}
*/
