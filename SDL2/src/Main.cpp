#include "Game.h"
#include "Config.h"
#include <SDL_image.h>

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	Game game("Chess Game", Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
	game.Execute();

	SDL_Quit();
	IMG_Quit();

	return 0;
}