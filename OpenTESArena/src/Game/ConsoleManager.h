#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <functional>
#include <string>
#include <vector>

// Console event manager. Contains all the lines in the console and handles console commands

class ConsoleManager
{
private:
	std::vector<std::string> commands;
	std::vector<std::function<void(const std::string&)>> command_functions;

	std::vector<std::string> contents;
	const int maxStrings = 16;

	void *game;
public:
	ConsoleManager();

	void putString(const std::string &str);
	std::string getString(int index);

	int numLines();

	void tryExecute(const std::string &command);

	void setGame(void *game);

	void addConsoleCommand(std::string name, std::function<void(const std::string&)> action);
};
#endif
