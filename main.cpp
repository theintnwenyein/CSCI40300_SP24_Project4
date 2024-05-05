/**
* Eagle: 10
* Members: Maria Colon, Theint Nwe Nyein, Leo Goldstein, Josh Moore
* 
* This file is the main file of the program. All other files are found
* within their respective file locations within the given directories
* 
* LOG:
* 1 March 2024
* - Added Header information
*/

//--- Imports ---//
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sys/wait.h>

#include "header_files/util.h" // String, Vector
#include "header_files/internal_commands.h"
#include "header_files/keywords.h"
#include "header_files/special_char.h"
#include "header_files/runnable.h"
#include "header_files/history_manager.h"

//--- Pre-cached methods ---//
std::string convert_to_string(int, char**);
cmd_type isInternalCommand(std::string);
bool isKeyword(std::string);
bool isFile(std::string);
void runFile(std::string);
void output_to_file(std::string, std::string, bool);

/**
* Name: main
* Type: Method
* Description: Main function of the program. This intakes the arguments of the program and
*              organizes the information into the correct command to be ran.
* Parameters:
*   argc INT - Number of arguments for the program. The program will always have at least one (1)
*   args char** - Double pointer containing all arguments given for the 
* Return:
*   INT - 0 if program runs correctly
*/
int main(int argc, char** args) {
    bool mode = true;
    bool hasExternal = false;
    cmd_type cmd;
    HistoryManager hm;

    std::string value(argc >= 2 ? args[1] : "");
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){return std::tolower(c); });

    if(argc < 2)
        std::cout << "Entering interactive mode..." << std::endl;
    else {
        if(split_string(value, " ")[1] == "if") {
            mode = false;
            hasExternal = true;
        } else {
            cmd = isInternalCommand(args[1]);
            mode = (cmd != 0);
        }

    }

    std::string uInput = convert_to_string(argc, args);

    if(mode) {
        while(true) {
            if(uInput == "") {
                std::cout << getCurrentDirectory() << " $ ";
                std::getline(std::cin, uInput);
                continue;
            } else if(split_string(uInput, " ")[0] == "(") {
		        std::string subString = uInput.substr(2, uInput.length() - 2);
		        std::vector<std::string> commands = split_string(subString, ";");

		        for(auto& s : commands) {
                    if(s == " )")
                        break;
                    
                    cmd = isInternalCommand(split_string(s, " ")[0]);

                    if(s == "exit" || s == "logout")
                        break;
                    else if(isKeyword(split_string(s, " ")[0]))
                        run_command(cmd, hm, s);
                    else if(cmd != none) {
                        if(cmd == cd) {
                            run_command_cd(hm, s);
                        } else
                            run_command(cmd, hm, s);
                    } else
                        runExternalCommand((char*)split_string(s, " ")[0].c_str(), s);
                }
		
		        std::cout << getCurrentDirectory() << " $ ";
    	        std::getline(std::cin, uInput);
		        continue;		
	        }
            
            cmd = isInternalCommand(split_string(uInput, " ")[0]);

            if(uInput == "exit" || uInput == "logout")
                break;
            else if(isKeyword(split_string(uInput, " ")[0]))
                run_command(cmd, hm, uInput);
            else if(cmd != none) {
                if(cmd == cd) {
                    run_command_cd(hm, uInput);
                } else
                    run_command(cmd, hm, uInput);
            } else if(uInput.c_str()[0] == '(') {
                std::cout << "Hit" << std::endl;
                run_command_subsys(cmd, hm, uInput); //TODO: Need to adjust for multi-command input
            } else
                runExternalCommand((char*)split_string(uInput, " ")[0].c_str(), uInput);

            std::cout << getCurrentDirectory() << " $ ";
            std::getline(std::cin, uInput);
        }
    } else { // Used for running batch mode
        if(isKeyword(split_string(uInput, " ")[0])) {
            run_command(cmd, hm, uInput);
        } else {
            runExternalCommand(split_string(uInput, " ")[0].c_str(), uInput);
        }
    }

    return 0;
}

/**
 * Name: convert_to_string
 * Type: Function
 * Parameters:
 *   argc INT - Number of arguments of the double char pointer
 *   args CHAR** - arguments given that will converge into a single string.
 *
 * Return:
 *   String
 */
std::string convert_to_string(int argc, char** args) {
    std::string ret;

    for(int i = 1; i < argc; i++)
        ret += (i + 1 < argc ? std::string(args[i]) + " " : std::string(args[i]));

    return ret;
}

/**
 * Name: isInternalCommand
 * Type: Function
 * Parameter:
 *   command STRING - first argument of user input to check.
 */
cmd_type isInternalCommand(std::string command) {
    if(command == "alias") return alias;
    else if(command == "bg") return bg;
    else if(command == "cd") return cd;
    else if(command == "eval") return eval;
    else if(command == "exec") return exec;
    else if(command == "export") return _export;
    else if(command == "fc") return fc;
    else if(command == "fg") return fg;
    else if(command == "help") return help;
    else if(command == "history") return history;
    else if(command == "jobs") return jobs;
    else if(command == "let") return let;
    else if(command == "local") return local;
    else if(command == "set") return set;
    else if(command == "shift") return shift;
    else if(command == "shopt") return shopt;
    else if(command == "source") return source;
    else if(command == "unalias") return unalias;
    else return none;
}

bool isKeyword(std::string arg) { return arg == "if"; }

bool isFile(std::string input) {
	std::fstream testFile(getCurrentDirectory() + "/" + split_string(input, " ")[0]);
	bool ret = false;

	if(testFile.is_open())
		ret = true;
	
	testFile.close();
	return ret;
}

void runFile(std::string input) {
	
}

void output_to_file(std::string filename, std::string args, bool append){
    if(append){
        std::fstream file;
        file.open(filename, std::ios_base::app);
        if(file.is_open()){
            file << args;
            file.flush();
        }else{
            std::cout << "could not append."<< std::endl;
        }
        file.close();
    }else{
        std::fstream file;
        file.open(filename);
        if(file.is_open()){
            file << args;
            file.flush();
        }else{
            std::cout << "could not output."<< std::endl;
        }
        file.close(); 
    }
}
