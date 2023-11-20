#ifndef Board_h
#define Board_h

#include <stdio.h>
#include <set>
#include <stack>

#include "Player.h"
#include "Piece.h"

class Board
{
public:
    Board();
    Board(std::set<Player*> pl, int t);
    int getTurn();
    Piece* getBoardEntry(int r, int c);
    Piece* findPiece(int x, int y);
    void setPlayers(std::set<Player*> pl);
    void updateBoardAfterMove(Piece* piece, int newRow, int newCol);

    
private:
    Piece* board[8][8];
    int turn_num;
    std::set<Player*> players;
};

#endif /* Board_h */
