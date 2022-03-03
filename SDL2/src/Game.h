#pragma once

#include <SDL.h>
#include "Board.h"

enum GameStatus {GameStarted, GameFinished};

class Game {
private:
	bool m_isRunning;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Event m_event;
	Board* m_board;
	GameStatus m_status;

public:
	Game(const char* name, int width, int height);
	~Game();

	void Execute();
	void Finish(std::string winner);
};