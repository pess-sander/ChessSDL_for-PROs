#pragma once

#include <string>
#include <SDL.h>
#include <vector>

enum PieceColor { PieceBlack, PieceWhite };
enum PieceType { PieceKing, PieceRook, PieceBishop, PieceQueen, PieceKnight, PiecePawn };
struct PiecePos
{
	int x, y;

	bool operator==(PiecePos pos);
};

class Piece
{
protected:
	PieceColor m_color;
	PieceType m_type;
	PiecePos m_position;
	SDL_Surface* m_surface;
	SDL_Rect m_text_rect;
	bool m_eaten, m_grabbing;
	int m_x, m_y;

	bool m_pawnDoubleMove;

public:
	Piece(PieceColor color, PieceType type, PiecePos position);
	~Piece();

	void draw(SDL_Renderer* renderer, SDL_Texture* atlas);
	void drawGrabbing(SDL_Renderer* renderer, SDL_Texture* atlas);

	PiecePos& getPos();

	bool isEaten();
	bool isBeingGrabbed();
	PieceColor getColor();
	PieceType getType();

	void grab(int x, int y);
	void startGrabbing();
	std::string stopGrabbing(std::vector<Piece>& pieces);

	void eat();

	std::vector<PiecePos> getPossibleMoves(std::vector<Piece>& pieces);

	bool operator==(Piece* p);
};