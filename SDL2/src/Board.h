#pragma once

#include <SDL.h>
#include <vector>
#include "Piece.h"

class Board
{
private:
	std::vector<Piece> pieces;
	SDL_Texture* atlas;

public:
	Board(PieceColor playerColor, SDL_Renderer* renderer);
	~Board();

	Piece* getPieceByPos(PiecePos pos);
	std::vector<Piece>& getPieces();

	void drawBoard(SDL_Renderer* renderer);
	void drawPieces(SDL_Renderer* renderer);
	void drawPossibleMoves(SDL_Renderer* renderer, std::vector<PiecePos> moves);
};