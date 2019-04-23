#include "ConsoleManager.h"

#include "SDL.h"

#include "Game.h"
#include "../Utilities/Debug.h"

#define CVAR_OPTIONS_BOOL(cvar_name, option_name, additional_lines) \
addConsoleCommand(#cvar_name, [this](const std::string &str) \
{ \
	Game* game = (Game*)this->game; \
\
	int (cvar_name) = [this, &str]() \
	{ \
		try \
		{ \
			return (std::stoi(str) != 0)?1:0; \
		} \
		catch (...) \
		{ \
			return -1; \
		} \
	}(); \
\
	if (cvar_name == -1) \
	{ \
		putString(#cvar_name" = " + std::to_string(game->getOptions().get##option_name())); \
		return; \
	} \
\
	game->getOptions().set##option_name(cvar_name == 1); \
	additional_lines; \
}); \
DebugMention("CVAR_OPTIONS_BOOL registred: "#cvar_name);

#define CVAR_OPTIONS_INT(cvar_name, option_name, additional_lines) \
addConsoleCommand(#cvar_name, [this](const std::string &str) \
{ \
	Game* game = (Game*)this->game; \
\
	int (cvar_name) = [this, &str]() \
	{ \
		try \
		{ \
			return std::stoi(str);\
		} \
		catch (...) \
		{ \
			return -1; \
		} \
	}(); \
\
	if (cvar_name == -1) \
	{ \
		putString(#cvar_name" = " + std::to_string(game->getOptions().get##option_name())); \
		return; \
	} \
\
	game->getOptions().set##option_name(cvar_name); \
	additional_lines; \
}); \
DebugMention("CVAR_OPTIONS_INT registred: "#cvar_name);

#define CVAR_OPTIONS_DOUBLE(cvar_name, option_name, additional_lines) \
addConsoleCommand(#cvar_name, [this](const std::string &str) \
{ \
	Game* game = (Game*)this->game; \
\
	double (cvar_name) = [this, &str]() \
	{ \
		try \
		{ \
			return std::stod(str);\
		} \
		catch (...) \
		{ \
			return -1.0; \
		} \
	}(); \
\
	if (cvar_name == -1.0) \
	{ \
		putString(#cvar_name" = " + std::to_string(game->getOptions().get##option_name())); \
		return; \
	} \
\
	game->getOptions().set##option_name(cvar_name); \
	additional_lines; \
}); \
DebugMention("CVAR_OPTIONS_DOUBLE registred: "#cvar_name);

#define CVAR_OPTIONS_STRING(cvar_name, option_name, additional_lines) \
addConsoleCommand(#cvar_name, [this](const std::string &str) \
{ \
	Game* game = (Game*)this->game; \
\
	const std::string cvar_name = str; \
\
	if (cvar_name == "") \
	{ \
		putString(#cvar_name" = " + game->getOptions().get##option_name()); \
		return; \
	} \
\
	game->getOptions().set##option_name(cvar_name); \
	additional_lines; \
}); \
DebugMention("CVAR_OPTIONS_STRING registred: "#cvar_name);

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
		DebugMention(str);
	});

	// Graphics cvars
	CVAR_OPTIONS_BOOL(r_fullscreen, Graphics_Fullscreen, game->getRenderer().setFullscreen(r_fullscreen == 1));

	CVAR_OPTIONS_INT(r_target_fps, Graphics_TargetFPS, );

	CVAR_OPTIONS_DOUBLE(r_resolution_scale, Graphics_ResolutionScale, );

	CVAR_OPTIONS_DOUBLE(r_vertical_fov, Graphics_VerticalFOV, );

	CVAR_OPTIONS_BOOL(r_parallax_sky, Graphics_ParallaxSky, );

	CVAR_OPTIONS_INT(r_letterbox_mode, Graphics_LetterboxMode, game->getRenderer().setLetterboxMode(r_letterbox_mode););

	CVAR_OPTIONS_DOUBLE(r_cursor_scale, Graphics_CursorScale, );

	CVAR_OPTIONS_BOOL(r_modern_interface, Graphics_ModernInterface, bool use_modern_interface = r_modern_interface == 1; \
		if (!use_modern_interface) \
		{ \
			auto &player = game->getGameData().getPlayer(); \
			const Double2 groundDirection = player.getGroundDirection(); \
			const Double3 lookAtPoint = player.getPosition() + Double3(groundDirection.x, 0.0, groundDirection.y); \
			player.lookAt(lookAtPoint); \
		} \

		auto &renderer = game->getRenderer(); \
		const Int2 windowDimensions = renderer.getWindowDimensions(); \
		renderer.resize(windowDimensions.x, windowDimensions.y, game->getOptions().getGraphics_ResolutionScale(), use_modern_interface); \
	);

	CVAR_OPTIONS_INT(r_threads_mode, Graphics_RenderThreadsMode, game->getRenderer().setRenderThreadsMode(r_threads_mode));

	// Audio cvars	
	CVAR_OPTIONS_DOUBLE(a_music_volume, Audio_MusicVolume, game->getAudioManager().setMusicVolume(game->getOptions().getAudio_MusicVolume()));

	CVAR_OPTIONS_DOUBLE(a_sound_volume, Audio_SoundVolume, game->getAudioManager().setSoundVolume(game->getOptions().getAudio_SoundVolume()));

	CVAR_OPTIONS_STRING(a_midi_config, Audio_MidiConfig, );

	CVAR_OPTIONS_INT(a_sound_channels, Audio_SoundChannels, );

	CVAR_OPTIONS_INT(a_sound_resampling, Audio_SoundResampling, \
		auto &audioManager = game->getAudioManager(); \
		if (audioManager.hasResamplerExtension()) \
		{ \
			audioManager.setResamplingOption(a_sound_resampling); \
		});

	// Input cvars
	CVAR_OPTIONS_DOUBLE(i_horisontal_sensivity, Input_HorizontalSensitivity, );

	CVAR_OPTIONS_DOUBLE(i_vertical_sensivity, Input_VerticalSensitivity, );

	CVAR_OPTIONS_DOUBLE(i_camera_pitch_limit, Input_CameraPitchLimit, );

	// Misc cvars
	CVAR_OPTIONS_STRING(m_arena_path, Misc_ArenaPath, );

	CVAR_OPTIONS_STRING(m_arena_saves_path, Misc_ArenaSavesPath, );

	CVAR_OPTIONS_BOOL(m_collision, Misc_Collision, );

	CVAR_OPTIONS_BOOL(m_skip_intro, Misc_SkipIntro, );

	CVAR_OPTIONS_BOOL(m_show_debug, Misc_ShowDebug, );

	CVAR_OPTIONS_BOOL(m_show_compass, Misc_ShowCompass, );

	CVAR_OPTIONS_DOUBLE(m_time_scale, Misc_TimeScale, );

	CVAR_OPTIONS_INT(m_star_density, Misc_StarDensity, );
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
			command_functions[i]((arg == name)?"":arg);
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
