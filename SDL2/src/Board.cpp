#include "Board.h"
#include "Config.h"
#include <SDL_image.h>
#include <algorithm>
#include <iostream>

Board::Board(PieceColor playerColor, SDL_Renderer* renderer)
{
	SDL_Surface* img = IMG_Load("assets/pieces.png");
	atlas = SDL_CreateTextureFromSurface(renderer, img);
	SDL_FreeSurface(img);

	PieceColor enemyColor = playerColor == PieceWhite ? PieceBlack : PieceWhite;


	pieces.reserve(32);
	pieces.emplace_back(Piece(enemyColor, PieceBishop, {1, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceRook, {2, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceKnight, {3, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceKing, {4, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceQueen, {5, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceKnight, {6, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceRook, {7, 1}));
	pieces.emplace_back(Piece(enemyColor, PieceBishop, {8, 1}));

	for (int i = 1; i <= 8; i++) {
		pieces.emplace_back(Piece(enemyColor, PiecePawn, { i, 2 }));
	}

	for (int i = 1; i <= 8; i++) {
		pieces.emplace_back(Piece(playerColor, PiecePawn, { i, 7 }));
	}

	pieces.emplace_back(Piece(playerColor, PieceBishop, {1, 8}));
	pieces.emplace_back(Piece(playerColor, PieceRook, { 2, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceKnight, { 3, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceKing, { 4, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceQueen, { 5, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceKnight, { 6, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceRook, { 7, 8 }));
	pieces.emplace_back(Piece(playerColor, PieceBishop, { 8, 8 }));
}

Board::~Board()
{
	SDL_DestroyTexture(atlas);
}

void Board::drawBoard(SDL_Renderer* renderer)
{
	SDL_Rect cell;
	cell.w = Config::CELL_WIDTH;
	cell.h = Config::CELL_WIDTH;
	
	SDL_Rect fullScreen;
	fullScreen.x = 0;
	fullScreen.y = 0;
	fullScreen.w = Config::WINDOW_WIDTH;
	fullScreen.h = Config::WINDOW_HEIGHT;
	SDL_SetRenderDrawColor(renderer, 213, 155, 67, 255);
	SDL_RenderFillRect(renderer, &fullScreen);
	SDL_SetRenderDrawColor(renderer, 157, 98, 9, 255);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2 == 0) {
				cell.x = i * Config::CELL_WIDTH;
				cell.y = j * Config::CELL_WIDTH;

				SDL_RenderFillRect(renderer, &cell);
			}
		}
	}
}

void Board::drawPieces(SDL_Renderer* renderer)
{
	Piece* p = nullptr;
	for (int i = 0; i < pieces.size(); i++) {
		if (!pieces[i].isEaten()) {
			if (!pieces[i].isBeingGrabbed())
				pieces[i].draw(renderer, atlas);
			else
				p = &pieces[i];
		}
	}

	if (p != nullptr)
		p->drawGrabbing(renderer, atlas);
}

void Board::drawPossibleMoves(SDL_Renderer* renderer, std::vector<PiecePos> moves)
{
	SDL_Rect cell;
	cell.w = Config::CELL_WIDTH;
	cell.h = Config::CELL_WIDTH;
	for (auto& it : moves)
	{
		cell.x = (it.x-1) * Config::CELL_WIDTH;
		cell.y = (it.y-1) * Config::CELL_WIDTH;
		if ((it.x + it.y) % 2 == 0)
			SDL_SetRenderDrawColor(renderer, 194, 121, 11, 255);
		else
			SDL_SetRenderDrawColor(renderer, 248, 180, 78, 255);

		SDL_RenderFillRect(renderer, &cell);
	}
}

Piece* Board::getPieceByPos(PiecePos pos)
{
	for (int i = 0; i < pieces.size(); i++)
	{
		if (pieces[i].getPos() == pos && !pieces[i].isEaten())
			return &pieces[i];
	}
	
	return nullptr;
}

std::vector<Piece>& Board::getPieces()
{
	return pieces;
}