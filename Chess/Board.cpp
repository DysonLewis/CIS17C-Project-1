#include "Board.h"
#include "Player.h"

Board::Board()
{
    // pieces are denoted: (p)awn, (k)night, (b)ishop, (r)ook, (q)ueen, (k)ing
    // black pieces are represented by uppercase, white pieces are represented by lowercase
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = nullptr;
        }
    }
    turn_num = 0;
}

Board::Board(std::set<Player*> pls, int t)
{
    turn_num = t;

    // Initialize the board
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            board[row][col] = nullptr;
        }
    }

    players = pls;
    
    // Iterate through the set of players
    for (const auto& player : pls)
    {
        // Iterate through the player's active pieces
        for (Piece* piece : player->getActivePieces())
        {
            // Get the type, row, and column of the piece
            int row = piece->getRow();
            int col = piece->getCol();

            // Update the board with the piece type
            board[row][col] = piece;
        }
    }
}


int Board::getTurn()
{
    return turn_num;
}

Piece* Board::getBoardEntry(int r, int c)
{
    return board[r][c];
}


Piece* Board::findPiece(int x, int y)
{
    if (board[x][y] == nullptr)
        return nullptr;
    for (const auto& player : players)
    {
        for (Piece* piece : player->getActivePieces())
        {
            if (board[x][y] == piece && x == piece->getRow() && y == piece->getCol())
            {
                return piece;
            }
        }
    }
    return nullptr;
}

void Board::setPlayers(std::set<Player*> pl)
{
    players = pl;
}

void Board::updateBoardAfterMove(Piece* piece, int newRow, int newCol) {
    int oldRow = piece->getRow();
    int oldCol = piece->getCol();

    // Clear the old position on the board
    board[oldRow][oldCol] = nullptr;

    // Update the piece's position
    piece->setRow(newRow);
    piece->setCol(newCol);

    // Update the board with the piece's new position
    board[newRow][newCol] = piece;
}
