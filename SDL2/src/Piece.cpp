#include "Piece.h"
#include <SDL_image.h>
#include <iostream>
#include "Config.h"

static Piece* getPieceByPos(PiecePos pos, std::vector<Piece>& pieces)
{
	for (int i = 0; i < pieces.size(); i++)
	{
		if (pieces[i].getPos() == pos && !pieces[i].isEaten())
			return &pieces[i];
	}

	return nullptr;
}

bool PiecePos::operator==(PiecePos pos)
{
	return (x == pos.x) && (y == pos.y);
}

Piece::Piece(PieceColor color, PieceType type, PiecePos position)
	: m_color(color), m_type(type), m_position(position), m_eaten(false), m_grabbing(false), m_pawnDoubleMove(true)
{
	m_text_rect.w = 200;
	m_text_rect.h = 200;
	switch (m_color)
	{
	case PieceWhite:
		m_text_rect.y = 0;
		break;
	case PieceBlack:
		m_text_rect.y = 200;
	}
	switch (m_type)
	{
	case PieceKing:
		m_text_rect.x = 0;
		break;
	case PieceQueen:
		m_text_rect.x = 200;
		break;
	case PieceRook:
		m_text_rect.x = 400;
		break;
	case PieceKnight:
		m_text_rect.x = 600;
		break;
	case PieceBishop:
		m_text_rect.x = 800;
		break;
	case PiecePawn:
		m_text_rect.x = 1000;
		break;
	}
}

Piece::~Piece()
{
}

void Piece::draw(SDL_Renderer* renderer, SDL_Texture* atlas)
{
	SDL_Rect rect;
	rect = { (m_position.x - 1) * Config::CELL_WIDTH, (m_position.y - 1) * Config::CELL_WIDTH, Config::CELL_WIDTH, Config::CELL_WIDTH };
	SDL_RenderCopy(renderer, atlas, &m_text_rect, &rect);
}

void Piece::drawGrabbing(SDL_Renderer* renderer, SDL_Texture* atlas)
{
	SDL_Rect rect;
	rect = { m_x, m_y, Config::CELL_WIDTH, Config::CELL_WIDTH };
	SDL_RenderCopy(renderer, atlas, &m_text_rect, &rect);
}

void Piece::startGrabbing()
{
	m_grabbing = true;

	m_x = (m_position.x - 1) * Config::CELL_WIDTH;
	m_y = (m_position.y - 1) * Config::CELL_WIDTH;
}

void Piece::grab(int x, int y)
{
	m_x += x;
	m_y += y;
}

std::string Piece::stopGrabbing(std::vector<Piece>& pieces)
{
	m_grabbing = false;

	PiecePos newPos = { (m_x + Config::CELL_WIDTH/2) / Config::CELL_WIDTH + 1, (m_y + Config::CELL_WIDTH/2) / Config::CELL_WIDTH + 1 };
	for (auto it : getPossibleMoves(pieces))
	{
		Piece* p = getPieceByPos(newPos, pieces);
		if (it == newPos)
		{
			m_position = newPos;
			if (m_type == PiecePawn && m_pawnDoubleMove == true) m_pawnDoubleMove = false;
			if (p != nullptr) {
				p->eat();
				if (p->getType() == PieceKing)
					return (getColor() == PieceWhite) ? "White" : "Black";
			}
			
			/*for (auto itt : getPossibleMoves(pieces))
			{
				if (getPieceByPos(itt, pieces) != nullptr)
					if (getPieceByPos(itt, pieces)->getType() == PieceKing) {
						return (getColor() == PieceWhite) ? "White" : "Black";
					}
			}*/

			return "";
		}
	}

	return "nobody";
}

PiecePos& Piece::getPos()
{
	return m_position;
}

std::vector<PiecePos> Piece::getPossibleMoves(std::vector<Piece>& pieces)
{
	std::vector<PiecePos> moves;

	switch (m_type)
	{
	case PiecePawn:
	{
		int k = (m_color == PieceWhite) ? -1 : +1;
		Piece* up = getPieceByPos({ m_position.x, m_position.y + k }, pieces);
		Piece* dUp = getPieceByPos({ m_position.x, m_position.y + k * 2 }, pieces);
		Piece* dUpR = getPieceByPos({ m_position.x + 1, m_position.y + 1 * k }, pieces);
		Piece* dUpL = getPieceByPos({ m_position.x - 1, m_position.y + 1 * k }, pieces);

		bool a = false;

			if (dUpR != nullptr) {
				if (dUpR->getColor() != getColor())
				{
					moves.push_back({ m_position.x + 1, m_position.y + 1 * k });
					a = true;
				}
			}
			if (dUpL != nullptr) {
				if (dUpL->getColor() != getColor())
				{
					moves.push_back({ m_position.x - 1, m_position.y + 1 * k });
					a = true;
				}
			}
			if (m_pawnDoubleMove && !a)
			{
				if (dUp == nullptr)
					moves.push_back({ m_position.x, m_position.y + k * 2 });
				if (up == nullptr)
					moves.push_back({ m_position.x, m_position.y + k });
			}
			else if (!a) {
				if (getPieceByPos({ m_position.x, m_position.y + k }, pieces) == nullptr)
					moves.push_back({ m_position.x, m_position.y + k });
			}
		break;
	}
	case PieceBishop:
		for (int i = m_position.x+1; i <= 8; i++) {
			Piece* p = getPieceByPos({ i, m_position.y }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ i, m_position.y });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ i, m_position.y });
				break;
			}
			else break;
		}

		for (int i = m_position.x - 1; i >= 1; i--) {
			Piece* p = getPieceByPos({ i, m_position.y }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ i, m_position.y });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ i, m_position.y });
				break;
			}
			else break;
		}

		for (int i = m_position.y + 1; i <= 8; i++) {
			Piece* p = getPieceByPos({ m_position.x, i }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ m_position.x, i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x, i });
				break;
			}
			else break;
		}

		for (int i = m_position.y - 1; i >= 1; i--) {
			Piece* p = getPieceByPos({ m_position.x, i }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ m_position.x, i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x, i });
				break;
			}
			else break;
		}
		break;
	case PieceRook:
		for (int i = 1; m_position.x + i <= 8 && m_position.y + i <= 8; i++)
		{
			Piece* p = getPieceByPos({ m_position.x + i, m_position.y + i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x + i, m_position.y + i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x + i, m_position.y + i });
				break;
			}
			else break;
		}

		for (int i = 1; m_position.x + i <= 8 && m_position.y - i >= 1; i++)
		{
			Piece* p = getPieceByPos({ m_position.x + i, m_position.y - i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x + i, m_position.y - i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x + i, m_position.y - i });
				break;
			}
			else break;
		}

		for (int i = 1; m_position.x - i >= 1 && m_position.y + i <= 8; i++)
		{
			Piece* p = getPieceByPos({ m_position.x - i, m_position.y + i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x - i, m_position.y + i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x - i, m_position.y + i });
				break;
			}
			else break;
		}

		for (int i = 1; m_position.x - i >= 1 && m_position.y - i >= 1; i++)
		{
			Piece* p = getPieceByPos({ m_position.x - i, m_position.y - i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x - i, m_position.y - i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x - i, m_position.y - i });
				break;
			}
			else break;
		}
		break;
	case PieceKnight:
	{
		Piece* upL = getPieceByPos({ m_position.x - 1, m_position.y - 2 }, pieces);
		Piece* upR = getPieceByPos({ m_position.x + 1, m_position.y - 2 }, pieces);
		Piece* leftU = getPieceByPos({ m_position.x - 2, m_position.y - 1 }, pieces);
		Piece* leftD = getPieceByPos({ m_position.x - 2, m_position.y + 1 }, pieces);
		Piece* downL = getPieceByPos({ m_position.x - 1, m_position.y + 2 }, pieces);
		Piece* downR = getPieceByPos({ m_position.x + 1, m_position.y + 2 }, pieces);
		Piece* rightU = getPieceByPos({ m_position.x + 2, m_position.y - 1 }, pieces);
		Piece* rightD = getPieceByPos({ m_position.x + 2, m_position.y + 1 }, pieces);

		if (m_position.x - 1 >= 1 && m_position.y - 2 >= 1) {
			if (upL == nullptr) {
				moves.push_back({ m_position.x - 1, m_position.y - 2 });
			}
			else if (upL->getColor() != getColor()) {
				moves.push_back({ m_position.x - 1, m_position.y - 2 });
			}
		}
		if (m_position.x + 1 <= 8 && m_position.y - 2 >= 1) {
			if (upR == nullptr) {
				moves.push_back({ m_position.x + 1, m_position.y - 2 });
			}
			else if (upR->getColor() != getColor()) {
				moves.push_back({ m_position.x + 1, m_position.y - 2 });
			}
		}
		if (m_position.x - 2 >= 1 && m_position.y - 1 >= 1) {
			if (leftU == nullptr) {
				moves.push_back({ m_position.x - 2, m_position.y - 1 });
			}
			else if (leftU->getColor() != getColor()) {
				moves.push_back({ m_position.x - 2, m_position.y - 1 });
			}
		}
		if (m_position.x - 2 >= 1 && m_position.y + 1 <= 8) {
			if (leftD == nullptr) {
				moves.push_back({ m_position.x - 2, m_position.y + 1 });
			}
			else if (leftD->getColor() != getColor()) {
				moves.push_back({ m_position.x - 2, m_position.y + 1 });
			}
		}
		if (m_position.x - 1 >= 1 && m_position.y + 2 <= 8) {
			if (downL == nullptr) {
				moves.push_back({ m_position.x - 1, m_position.y + 2 });
			}
			else if (downL->getColor() != getColor()) {
				moves.push_back({ m_position.x - 1, m_position.y + 2 });
			}
		}
		if (m_position.x + 1 <= 8 && m_position.y + 2 <= 8) {
			if (downR == nullptr) {
				moves.push_back({ m_position.x + 1, m_position.y + 2 });
			}
			else if (downR->getColor() != getColor()) {
				moves.push_back({ m_position.x + 1, m_position.y + 2 });
			}
		}
		if (m_position.x + 2 <= 8 && m_position.y - 1 >= 1) {
			if (rightU == nullptr) {
				moves.push_back({ m_position.x + 2, m_position.y - 1 });
			}
			else if (rightU->getColor() != getColor()) {
				moves.push_back({ m_position.x + 2, m_position.y - 1 });
			}
		}
		if (m_position.x + 2 <= 8 && m_position.y + 1 <= 8) {
			if (rightD == nullptr) {
				moves.push_back({ m_position.x + 2, m_position.y + 1 });
			}
			else if (rightD->getColor() != getColor()) {
				moves.push_back({ m_position.x + 2, m_position.y + 1 });
			}
		}
		break;
	}
	case PieceKing:
	{
		Piece* u = getPieceByPos({ m_position.x, m_position.y - 1 }, pieces);
		Piece* ur = getPieceByPos({ m_position.x + 1, m_position.y - 1 }, pieces);
		Piece* r = getPieceByPos({ m_position.x + 1, m_position.y }, pieces);
		Piece* dr = getPieceByPos({ m_position.x + 1, m_position.y + 1 }, pieces);
		Piece* d = getPieceByPos({ m_position.x, m_position.y + 1 }, pieces);
		Piece* dl = getPieceByPos({ m_position.x - 1, m_position.y + 1 }, pieces);
		Piece* l = getPieceByPos({ m_position.x - 1, m_position.y }, pieces);
		Piece* ul = getPieceByPos({ m_position.x - 1, m_position.y - 1 }, pieces);

		if (m_position.y - 1 >= 1) {
			if (u == nullptr)
				moves.push_back({ m_position.x, m_position.y - 1 });
			else if (u->getColor() != getColor())
				moves.push_back({ m_position.x, m_position.y - 1 });
		}
		if (m_position.y - 1 >= 1 && m_position.x + 1 <= 8) {
			if (ur == nullptr)
				moves.push_back({ m_position.x + 1, m_position.y - 1 });
			else if (ur->getColor() != getColor())
				moves.push_back({ m_position.x + 1, m_position.y - 1 });
		}
		if (m_position.x + 1 <= 8) {
			if (r == nullptr)
				moves.push_back({ m_position.x + 1, m_position.y });
			else if (r->getColor() != getColor())
				moves.push_back({ m_position.x + 1, m_position.y });
		}
		if (m_position.x + 1 <= 8 && m_position.y + 1 <= 8) {
			if (dr == nullptr)
				moves.push_back({ m_position.x + 1, m_position.y + 1 });
			else if (dr->getColor() != getColor())
				moves.push_back({ m_position.x + 1, m_position.y + 1 });
		}
		if (m_position.y + 1 <= 8) {
			if (d == nullptr)
				moves.push_back({ m_position.x, m_position.y + 1 });
			else if (d->getColor() != getColor())
				moves.push_back({ m_position.x, m_position.y + 1 });
		}
		if (m_position.y + 1 <= 8 && m_position.x - 1 >= 1) {
			if (dl == nullptr)
				moves.push_back({ m_position.x - 1, m_position.y + 1 });
			else if (dl->getColor() != getColor())
				moves.push_back({ m_position.x - 1, m_position.y + 1 });
		}
		if (m_position.x - 1 >= 1) {
			if (l == nullptr)
				moves.push_back({ m_position.x - 1, m_position.y });
			else if (l->getColor() != getColor())
				moves.push_back({ m_position.x - 1, m_position.y });
		}
		if (m_position.y - 1 >= 1 && m_position.x - 1 >= 1) {
			if (ul == nullptr)
				moves.push_back({ m_position.x - 1, m_position.y - 1 });
			else if (ul->getColor() != getColor())
				moves.push_back({ m_position.x - 1, m_position.y - 1 });
		}
		break;
	}
	case PieceQueen:
		for (int i = 1; m_position.x + i <= 8 && m_position.y + i <= 8; i++)
		{
			Piece* p = getPieceByPos({ m_position.x + i, m_position.y + i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x + i, m_position.y + i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x + i, m_position.y + i });
				break;
			}
			else break;
		}
		for (int i = 1; m_position.x + i <= 8 && m_position.y - i >= 1; i++)
		{
			Piece* p = getPieceByPos({ m_position.x + i, m_position.y - i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x + i, m_position.y - i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x + i, m_position.y - i });
				break;
			}
			else break;
		}
		for (int i = 1; m_position.x - i >= 1 && m_position.y + i <= 8; i++)
		{
			Piece* p = getPieceByPos({ m_position.x - i, m_position.y + i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x - i, m_position.y + i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x - i, m_position.y + i });
				break;
			}
			else break;
		}
		for (int i = 1; m_position.x - i >= 1 && m_position.y - i >= 1; i++)
		{
			Piece* p = getPieceByPos({ m_position.x - i, m_position.y - i }, pieces);
			if (p == nullptr) {
				moves.push_back({ m_position.x - i, m_position.y - i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x - i, m_position.y - i });
				break;
			}
			else break;
		}
		for (int i = m_position.x + 1; i <= 8; i++) {
			Piece* p = getPieceByPos({ i, m_position.y }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ i, m_position.y });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ i, m_position.y });
				break;
			}
			else break;
		}
		for (int i = m_position.x - 1; i >= 1; i--) {
			Piece* p = getPieceByPos({ i, m_position.y }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ i, m_position.y });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ i, m_position.y });
				break;
			}
			else break;
		}
		for (int i = m_position.y + 1; i <= 8; i++) {
			Piece* p = getPieceByPos({ m_position.x, i }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ m_position.x, i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x, i });
				break;
			}
			else break;
		}
		for (int i = m_position.y - 1; i >= 1; i--) {
			Piece* p = getPieceByPos({ m_position.x, i }, pieces);
			if (p == nullptr)
			{
				moves.push_back({ m_position.x, i });
			}
			else if (p->getColor() != getColor()) {
				moves.push_back({ m_position.x, i });
				break;
			}
			else break;
		}
		break;
	}

	return moves;
}

bool Piece::operator==(Piece* p)
{
	return (m_color == p->m_color) && (m_type == p->m_type) && (m_position == p->m_position);
}

bool Piece::isEaten()
{
	return m_eaten;
}

bool Piece::isBeingGrabbed()
{
	return m_grabbing;
}

PieceColor Piece::getColor()
{
	return m_color;
}

PieceType Piece::getType()
{
	return m_type;
}

void Piece::eat()
{
	m_eaten = true;
}