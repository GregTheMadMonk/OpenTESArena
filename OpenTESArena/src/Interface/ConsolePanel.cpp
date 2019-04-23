#include "ConsolePanel.h"

#include "CursorAlignment.h"
#include "TextAlignment.h"
#include "../Game/Game.h"
#include "../Interface/GameWorldPanel.h"
#include "../Media/Color.h"
#include "../Media/FontName.h"
#include "../Media/PaletteFile.h"
#include "../Media/PaletteName.h"
#include "../Media/TextureFile.h"
#include "../Media/TextureName.h"
#include "../Rendering/Texture.h"
#include "../Utilities/Debug.h"

ConsolePanel::ConsolePanel(Game &game) : Panel(game)
{
       this->buffer = "";	
}

std::pair<SDL_Texture*, CursorAlignment> ConsolePanel::getCurrentCursor() const
{
	auto &game = this->getGame();
	auto &renderer = game.getRenderer();
	auto &textureManager = game.getTextureManager();
	const auto &texture = textureManager.getTexture(
			TextureFile::fromName(TextureName::SwordCursor),
			PaletteFile::fromName(PaletteName::Default), renderer);
	return std::make_pair(texture.get(), CursorAlignment::TopLeft);
}

void ConsolePanel::handleEvent(const SDL_Event &e)
{
	auto &game = this->getGame();
	const auto &inputManager = game.getInputManager();
	const bool escapePressed = inputManager.keyPressed(e, SDLK_ESCAPE);
	const bool enterPressed = inputManager.keyPressed(e, SDLK_RETURN);
	const bool backspacePressed = inputManager.keyPressed(e, SDLK_BACKSPACE);

	if (escapePressed)
	{
		game.setPanel<GameWorldPanel>(game);	
	}
	else if (enterPressed)
	{
		ConsoleManager &consoleManager = game.getConsoleManager();
		consoleManager.tryExecute(buffer);
		this->buffer = "";
	}
	else if (backspacePressed)
	{
		buffer = buffer.substr(0, buffer.length() - 1);
	}
	else if (e.type == SDL_TEXTINPUT)
	{
	 	if (strcmp(e.text.text, "`") != 0) 
		{
			buffer += e.text.text; // ignore the '`' key, it triggers when we enter the console
		}
	}
}

void ConsolePanel::render(Renderer &renderer)
{
	renderer.clear();

	const Color backgroundColor(60, 60, 68);
	renderer.clearOriginal(backgroundColor);

	Game &game = this->getGame();

	const int centerCoordX = 160;

	const Int2 center(centerCoordX, 15);

	const RichTextString richText(
		">" + this->buffer + " ",
		FontName::Char,
		Color::Yellow,
		TextAlignment::Center, // Left acts as Center too for me for no reason
		game.getFontManager());

	const auto inputBox = std::make_unique<TextBox>(center, richText, game.getRenderer());

	renderer.drawOriginal(inputBox->getTexture(),
			inputBox->getX(), inputBox->getY());

	ConsoleManager consoleManager = game.getConsoleManager();

	for (int i = 0; i < consoleManager.numLines();  i++)
	{	
		const Int2 center2(centerCoordX, 15 + (consoleManager.numLines() - i) * 15);

		const RichTextString richText2(
			consoleManager.getString(i),
			FontName::Char,
			Color::White,
			TextAlignment::Left,
			game.getFontManager());

		const auto lineBox = std::make_unique<TextBox>(center2, richText2, game.getRenderer());

		renderer.drawOriginal(lineBox->getTexture(),
				lineBox->getX(), lineBox->getY());
	}
}
