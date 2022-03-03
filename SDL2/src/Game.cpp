#include "Game.h"
#include "Config.h"
#include <iostream>

Game::Game(const char* name, int width, int height)
	: m_isRunning(true), m_status(GameStarted)
{
	m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	m_board = new Board(PieceWhite, m_renderer);
}

Game::~Game()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	delete m_board;
}

void Game::Execute()
{
	SDL_RenderClear(m_renderer);
	m_board->drawBoard(m_renderer);
	m_board->drawPieces(m_renderer);
	SDL_RenderPresent(m_renderer);

	bool leftMousePressed = false;
	Piece* pGrabbing = nullptr;
	std::vector<PiecePos> moves;

	PieceColor turn = PieceWhite;

	std::string winner = "";

	while (m_isRunning) {
		while (SDL_WaitEvent(&m_event))
		{
			switch (m_event.type)
			{
			case SDL_QUIT:
				m_isRunning = false;
				return;

			case SDL_MOUSEBUTTONDOWN:
				if (m_status == GameStarted) {
					if (!leftMousePressed && m_event.button.button == SDL_BUTTON_LEFT) {
						pGrabbing = m_board->getPieceByPos({ m_event.button.x / Config::CELL_WIDTH + 1, m_event.button.y / Config::CELL_WIDTH + 1 });
						if (pGrabbing != nullptr) {
							if (pGrabbing->isEaten() || pGrabbing->getColor() != turn)
							{
								pGrabbing = nullptr;
							}
							else {
								pGrabbing->startGrabbing();
								leftMousePressed = true;

								moves = pGrabbing->getPossibleMoves(m_board->getPieces());
							}
						}
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (m_status == GameStarted) {
					if (leftMousePressed && m_event.button.button == SDL_BUTTON_LEFT) {
						leftMousePressed = false;
						winner = pGrabbing->stopGrabbing(m_board->getPieces());

						if (winner == "nobody")
							winner = "";
						else
							turn = (turn == PieceWhite) ? PieceBlack : PieceWhite;

						pGrabbing = nullptr;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (m_status == GameStarted) {
					if (leftMousePressed && pGrabbing != nullptr) {
						pGrabbing->grab(m_event.motion.xrel, m_event.motion.yrel);
					}
				}
				break;
			}
			
			if (m_status == GameStarted) {
				SDL_RenderClear(m_renderer);
				m_board->drawBoard(m_renderer);
				if (pGrabbing != nullptr) m_board->drawPossibleMoves(m_renderer, moves);
				m_board->drawPieces(m_renderer);
				SDL_RenderPresent(m_renderer);
			}

			if (winner != "" && m_status == GameStarted)
				Finish(winner);

			SDL_Delay(10);
		}
	}
}

void Game::Finish(std::string winner)
{
	std::cout << winner << " wins!" << std::endl;

	m_status = GameFinished;
}