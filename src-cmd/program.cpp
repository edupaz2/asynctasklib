#include <iostream>
#include <string>
#include <sstream>

#include "asynctasklib_defs.h"
#include "asynctasklib.h"

void printCommands()
{
    std::cout << "#-----------------#" << std::endl;
    std::cout << "Commands available:" << std::endl;
    std::cout << " start"                   << "\t\t\t\tStarts a task and prints its ID." << std::endl;
    std::cout << " start <task_type_id>"    << "\t\tStarts a task of a given type and prints its ID." << std::endl;
    std::cout << " pause <task_id>"         << "\t\tPause the task with the given ID and print a confirmation message." << std::endl;
    std::cout << " resume <task_id>"        << "\t\tResume task with the given ID (if paused) and print a confirmation message." << std::endl;
    std::cout << " stop <task_id>"          << "\t\t\tStop the task with the given ID (if not stopped) and print a confirmation message." << std::endl;
    std::cout << " status"                  << "\t\t\t\tPrints the ID, type ID, the status (paused, running, stopped, completed) and an optional indicator of progress for each task." << std::endl;
    std::cout << " status <task_id>"        << "\t\tPrints the ID, type ID, the status (paused, running, stopped, completed) and an optional indicator of progress for the task with the given ID." << std::endl;
    std::cout << " quit"                    << "\t\t\t\tQuit gracefully." << std::endl;
    std::cout << "#-----------------#" << std::endl;
}

void printStatus(const task_status& status)
{
	std::cout << "The status is" << std::endl;
}

void printCommandOutput(task_id id, const std::string& cmd)
{
	if(id >= FIRST_TASK_ID)
    {
		std::cout << "Command: " << cmd << " successful. Task [" << id << "]." << std::endl;
    }
    else
    {
		std::cout << "Oh oh. There was a problem when executing command " << cmd << "." << std::endl;
    }
}

task_id getTaskId(const std::string& cmd)
{
    try
    {
        return std::stoi(cmd); // TODO implement exception
    }
    catch(...) {}
    return 0;
}

int main(int argc, char* argv[]) {
	// Check arguments
	if(argc > 1)
	{
		// No need to check if arg[1] is help or not. Just print it anyways
        std::cout << "Usage: " << argv[0] << std::endl;
        std::cout << "Program will be launched and waiting for commands from the standard input" << std::endl;
        printCommands();
        return 0;
	}

    while (true)
    {
        std::string line;
        std::cout << ">>> Enter Command >>> ";
        std::getline(std::cin, line);

		std::vector<std::string> inputCmd;
		std::istringstream iss(line);
	    do
	    {
	        std::string subs;
	        iss >> subs;
            if(subs.size())
	           inputCmd.push_back(subs);
	    } while (iss);

	    int cmd_count = inputCmd.size();
        if(cmd_count > 0 && cmd_count <= 2)
	    {
	    	if(inputCmd[0] == "start")
            {
                printCommandOutput(startTask(), inputCmd[0]);
                continue;
            }
            else if(inputCmd[0] == "pause")
            {
                if(cmd_count == 2)
                {
                    printCommandOutput(pauseTask(getTaskId(inputCmd[1])), inputCmd[0]);
                    continue;
                }
            }
            else if(inputCmd[0] == "resume")
            {
                if(cmd_count == 2)
                {
                    printCommandOutput(resumeTask(getTaskId(inputCmd[1])), inputCmd[0]);
                    continue;
                }
            }
            else if(inputCmd[0] == "stop")
            {
                if(cmd_count == 2)
                {
                    printCommandOutput(stopTask(getTaskId(inputCmd[1])), inputCmd[0]);
                    continue;
                }
            }
            else if(inputCmd[0] == "status")
            {
                if(cmd_count == 1)
                {
                    printStatus(status());
                    continue;
                }
            }
            else if(inputCmd[0] == "quit")
            {
                if(cmd_count == 1)
                {
                    std::cout << "QUIT" << std::endl;
                    quit();
                    goto quit;
                }
            }
            else
            {
            	goto bad_cmd;
            }
	    }
bad_cmd:
    	// Something went wrong
    	printCommands();
    	continue;
    }

quit:
	std::cout << "Exiting" << std::endl;
    return 0;
}