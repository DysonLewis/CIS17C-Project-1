#ifndef Game_h
#define Game_h

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <string>

#include "Board.h"
#include "Player.h"
#include "Piece.h"

class Game
{
public:
    Game();
    ~Game();
    
    // acccessors
    void printBoard();
    int getTurnNum();
    bool isFinished();
    Player* getCurPlayer();
    Player* getOpponent();
    bool isKingSurrounded(Player* opponentPlayer, std::pair<int, int> unprotectedSquares[0]);
    bool simulateCheck(Player* player, int newRow, int newCol);
    
    
    bool isValidMove(Player* player, int sx, int sy, int ex, int ey);
    bool isInLineOfSight(int sx, int sy, int ex, int ey);
    void move(Player* player, int sx, int sy, int ex, int ey);
    void switchTurn();
    void setFinished();
    void undo();
    
    bool inCheckMate();
    bool inCheck();
    
    void start();
    
private:
    Board m_board;
    int tot_turns;
    bool finished;
    std::queue<Player*> players;
    std::stack<Board> history;
    bool isInLineOfSightOfKing(Piece* startPiece, Piece* endPiece);
    
    // helper
    Piece* findPiece(int x, int y);
};

#endif /* Game_h */
