#include "ConsoleManager.h"

#include "SDL.h"

#include "Game.h"
#include "../Utilities/Debug.h"

ConsoleManager::ConsoleManager()
{ 
	addConsoleCommand("exit", [](const std::string &literally_nothing) 
	{	
		SDL_Event evt;
		evt.quit.type = SDL_QUIT;
		evt.quit.timestamp = 0.0;
		SDL_PushEvent(&evt);
	});

	addConsoleCommand("echo", [](const std::string &str)
	{
		DebugWarning(str);
	});

	addConsoleCommand("r_fullscreen", [this](const std::string &str)
	{
		Game* game = (Game*)this->game;

		bool r_fullscreen = [this, &str]()
		{
			try 
			{
				return (std::stoi(str) != 0);
			}
			catch (...)
			{
				return false;
			}
		}();

		game->getOptions().setGraphics_Fullscreen(r_fullscreen);
		game->getRenderer().setFullscreen(r_fullscreen);
	});
}

void ConsoleManager::putString(const std::string &str)
{
	// remove the first entry if limit is reached
	if (this->contents.size() == this->maxStrings) this->contents.erase(this->contents.begin());

	this->contents.push_back(str);
}

std::string ConsoleManager::getString(int index)
{
	return contents.at(index);
}

int ConsoleManager::numLines()
{
	return this->contents.size();
}

void ConsoleManager::tryExecute(const std::string &command)
{
	Game *game = static_cast<Game*>(this->game);
		
	putString(">" + command);

	const int first_space = command.find(" ") + 1;
	const std::string name = command.substr(0, first_space - 1);
	const std::string arg = command.substr(first_space);

	for (int i = 0; i < commands.size(); i++)
	{
		if (commands[i] == name)
		{
			command_functions[i](arg);
			return;
		}
	}

	putString("\"" + command + "\" - command not found");
}

void ConsoleManager::setGame(void *game)
{
	this->game = game;
}

void ConsoleManager::addConsoleCommand(std::string name, std::function<void(const std::string&)> action)
{
	commands.push_back(name);
	command_functions.push_back(action);
}
