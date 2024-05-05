#ifndef H_RUNNABLECOMMANDS
#define H_RUNNABLECOMMANDS

#include "internal_commands.h"
#include "history_manager.h"

bool run_command(cmd_type, HistoryManager hm, std::string);
bool run_command_cd(HistoryManager, std::string);
bool runExternalCommand(const char*, std::string);
bool run_command_subsys(cmd_type, HistoryManager &hm, std::string);
#endif