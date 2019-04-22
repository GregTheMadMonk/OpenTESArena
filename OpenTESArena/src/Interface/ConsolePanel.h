#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include <string>

#include "Panel.h"

class Renderer;

class ConsolePanel : public Panel
{
private:
	std::string buffer;
public:
	ConsolePanel(Game &game);

	virtual ~ConsolePanel() = default;

	virtual std::pair<SDL_Texture*, CursorAlignment> getCurrentCursor() const override;
	virtual void handleEvent(const SDL_Event &e) override;
	virtual void render(Renderer &renderer) override;
};

#endif
