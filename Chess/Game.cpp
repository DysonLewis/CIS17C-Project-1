#include "Game.h"

#include <cmath>
#include <utility>

Game::Game() {
    m_board = Board();
    tot_turns = 0;
    finished = false;

    Player* white = new Player("white");
    players.push(white);
    Player* black = new Player("black");
    players.push(black);

    std::set<Player*> player_set;
    player_set.insert(white);
    player_set.insert(black);
    m_board.setPlayers(player_set);

    history.push(m_board);
}

Game::~Game() {
    // iterate through players and deallocate memory
    while (!players.empty()) {
        Player * p = players.front();
        players.pop();
        delete(p);
    }
}

// acccessors

void Game::printBoard() {
    std::cout << "   A B C D E F G H\n";
    std::cout << "  ----------------\n";

    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << "| ";
        for (int col = 0; col < 8; col++) {
            if (m_board.getBoardEntry(row, col) != nullptr)
                std::cout << m_board.getBoardEntry(row, col)->getType() << " ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int Game::getTurnNum() {
    return tot_turns;
}

bool Game::isFinished() {
    return finished;
}

Player* Game::getCurPlayer() {
    return players.front();
}

Player* Game::getOpponent() {
    return players.back();
}

bool Game::isValidMove(Player* player, int sx, int sy, int ex, int ey) {
    Piece* movingPiece = m_board.findPiece(sx, sy);

    // Check if the starting position has a piece
    if (movingPiece == nullptr || movingPiece->getType() == '-')
        return false;

    // Check if it's the player's own piece
    if (movingPiece->getType() >= 'a' && movingPiece->getType() <= 'z' && player->getColor() != "white")
        return false;
    if (movingPiece->getType() >= 'A' && movingPiece->getType() <= 'Z' && player->getColor() != "black")
        return false;

    // Find the piece at the destination
    Piece* destinationPiece = m_board.findPiece(ex, ey);

    //Check if the destination is occupied by the player's own piece
    if (destinationPiece != nullptr && destinationPiece->getType() != '-') {
        if ((destinationPiece->getType() >= 'a' && destinationPiece->getType() <= 'z' && player->getColor() == "white") ||
                (destinationPiece->getType() >= 'A' && destinationPiece->getType() <= 'Z' && player->getColor() == "black")) {
            return false;
        }
    }

    //pawn
    if (movingPiece->getType() == 'P' || movingPiece->getType() == 'p') {
        // Pawns capture diagonally, need to check first this first since other verifications override this
        if (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 1) {
            return true;
        }
        // Pawns move forward
        if (ey == sy) {
            if (((player->getColor() == "white") && (ex == sx - 1)) || ((player->getColor() == "black") && (ex == sx + 1))) {
                // Check if the destination square is unoccupied
                if (destinationPiece == nullptr) {
                    return true;
                } else {
                    std::cout << "Destination square is occupied\n";
                    return false;
                }
            }

            // Allow only one square move for subsequent moves
            if (movingPiece->getHasMoved() && std::abs(ex - sx) == 1) {
                if (destinationPiece == nullptr) {
                    return true;
                } else {
                    std::cout << "Destination square is occupied\n";
                    return false;
                }
            }
        }

        // Pawns move two squares on their first move
        if (!movingPiece->getHasMoved() && ey == sy && ((player->getColor() == "white" && ex == sx - 2) || (player->getColor() == "black" && ex == sx + 2))) {
            // Check if the squares in between are unoccupied
            int middleRow = (player->getColor() == "white") ? sx - 1 : sx + 1;
            return m_board.getBoardEntry(middleRow, ey) == nullptr && destinationPiece == nullptr;
        }


    }

    // Knight
    if (movingPiece->getType() == 'N' || movingPiece->getType() == 'n') {
        // Knights move in an L-shape (two squares in one direction and one square perpendicular)
        return (std::abs(ex - sx) == 2 && std::abs(ey - sy) == 1) || (std::abs(ex - sx) == 1 && std::abs(ey - sy) == 2);
    }

    // Only knight can jump -> all other piece need line of sight to where they are moving
    if (isInLineOfSight(sx, sy, ex, ey)) {

        // Rook
        if (movingPiece->getType() == 'R' || movingPiece->getType() == 'r') {
            // Rooks can move horizontally or vertically
            return (sx == ex || sy == ey);
        }

        // Bishop
        if (movingPiece->getType() == 'B' || movingPiece->getType() == 'b') {
            // Bishops move diagonally
            return std::abs(ex - sx) == std::abs(ey - sy);
        }

        // Queen
        if (movingPiece->getType() == 'Q' || movingPiece->getType() == 'q') {
            // Queens can move horizontally, vertically, or diagonally
            return (sx == ex || sy == ey || std::abs(ex - sx) == std::abs(ey - sy));
        }

        // King
        if (movingPiece->getType() == 'K' || movingPiece->getType() == 'k') {
            std::cout << sy << " " << ey << " " << sx << std::endl;
            // Hard coded castles
            // Black short
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 6 && sx == 0) {
                move(player, sx, 7, ex, 5); // Recursive call wooooo recursive call with O(1) time complexity
                return true; //since move length > 1, just return true as this would give a invalid move otherwise
            }
            // White short
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 6 && sx == 7) {
                move(player, sx, 7, ex, 5); // Recursive call wooooo recursive call with O(1) time complexity
                return true;
            }
            // Black long
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 1 && sx == 0) {
                move(player, sx, 0, ex, 2); // Recursive call wooooo recursive call with O(1) time complexity
                return true;
            }
            // White long
            if (!movingPiece->getHasMoved() && sy == 4 && ey == 1 && sx == 7) {
                move(player, sx, 0, ex, 2); // Recursive call wooooo recursive call with O(1) time complexity
                return true;
            }

            // Kings can move one square in any direction
            return (std::abs(ex - sx) <= 1 && std::abs(ey - sy) <= 1);
        }
    }
    // If none of the conditions failed, consider it a valid move
    return false;
}

void Game::move(Player* player, int sx, int sy, int ex, int ey) {
    // Find piece at the starting position
    Piece* movingPiece = m_board.findPiece(sx, sy);

    // Capture the piece at the destination if it exists
    Piece* destinationPiece = m_board.findPiece(ex, ey);
    if (destinationPiece != nullptr) {
        player->updatePiece(destinationPiece);
    }

    // Update the position of the moving piece on the current board
    movingPiece->setRow(ex);
    movingPiece->setCol(ey);
    movingPiece->setHasMoved(true);

    // Push the current board state to the history stack
    history.push(m_board);

    std::queue<Player*> tempQueue = players; // Create a copy of the original queue
    std::set<Player*> playerSet;

    while (!tempQueue.empty()) {
        playerSet.insert(tempQueue.front());
        tempQueue.pop();
    }

    // Create a new Board, set m_board to this new board
    m_board = Board(playerSet, tot_turns);
    // Increment tot_turns at the end
    tot_turns++;

    // Display the updated board with the move
    printBoard();
}

void Game::switchTurn() {
    Player* just_moved = players.front();
    players.pop();
    players.push(just_moved);
}

void Game::setFinished() {
    finished = true;
}

void Game::undo() {
    if (history.size() < 3) {
        return;
    }
    history.pop();
    m_board = history.top();
    switchTurn();
}

bool Game::inCheckMate() {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();
    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();
    int checkCount = 0, validMoves = 0;

    // Iterate through the squares around the king
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                // Skip the center square (king's position)
                continue;
            }

            int newRow = kingRow + i;
            int newCol = kingCol + j;


            // Check if the new position is within the board boundaries
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                Piece* destinationPiece = m_board.findPiece(newRow, newCol);

                // Check if the square is unoccupied
                if (destinationPiece == nullptr) {
                    // Simulate moving the king to the new position
                    m_board.updateBoardAfterMove(opKing, newRow, newCol);
                    printBoard();
                    validMoves++;

                    // Call simulateCheck with the coordinates of the simulated move
                    if (simulateCheck(opPlayer, newRow, newCol)) {
                        checkCount++; //Since king is still in check, iterate the count
                    }
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                }
            }
        }
    }
    if (checkCount == validMoves) {
        return true;
    } else {
        return false;
    }
}

bool Game::simulateCheck(Player* player, int newRow, int newCol) {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();

    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        std::cout << "King not found." << std::endl;
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();

    // Simulate moving the opponent's king to the new position
    m_board.updateBoardAfterMove(opKing, newRow, newCol);

    //Look if simulated move puts king in check
    std::pair<int, int> unprotectedSquares[1]; // array for empty squares around king, if found check if opponent piece is attacking king

    // Check if the opponent's pieces can attack the king
    if (!isKingSurrounded(opPlayer, unprotectedSquares)) {
        size_t size = sizeof (unprotectedSquares) / sizeof (unprotectedSquares[0]);

        // Check for line of sight for each unprotected square
        for (int i = 0; i < size; ++i) {
            int row = newRow;
            int col = newCol;

            // Check the entire column for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                Piece* curPiece = m_board.findPiece(r, col);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, opPlayer->getKing())) {
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                    return true; // The king is in check
                }
            }

            // Check the entire row for opponent pieces that can attack the king
            for (int c = 0; c < 8; ++c) {
                Piece* curPiece = m_board.findPiece(row, c);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    // Undo the simulated move
                    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                    return true; // The king is in check
                }
            }

            // Check the diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col + (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        // Undo the simulated move
                        m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
                        return true; // The king is in check
                    }
                }
            }
            for (int r = 0; r < 8; ++r) {
                int c = col - (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
        }
    }



    // Undo the simulated move
    m_board.updateBoardAfterMove(opKing, kingRow, kingCol);
    return false;
}

bool Game::inCheck() {
    Player* curPlayer = getCurPlayer();
    Player* opPlayer = getOpponent();

    Piece* opKing = opPlayer->getKing();

    if (opKing == nullptr) {
        // Handle the case where the king is not found
        std::cout << "King not found." << std::endl;
        return false;
    }

    int kingRow = opKing->getRow();
    int kingCol = opKing->getCol();


    std::pair<int, int> unprotectedSquares[1]; // array for empty squares around king, if found check if opponent piece is attacking king

    size_t size = sizeof (unprotectedSquares) / sizeof (unprotectedSquares[0]);

    // Check if the opponent's pieces can attack the king
    if (!isKingSurrounded(opPlayer, unprotectedSquares)) {
        // Check for line of sight for each unprotected square
        for (int i = 0; i < size; ++i) {
            int row = kingRow;
            int col = kingCol;

            // Check the entire column for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                Piece* curPiece = m_board.findPiece(r, col);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    return true; // The king is in check
                }
            }

            // Check the entire row for opponent pieces that can attack the king
            for (int c = 0; c < 8; ++c) {
                Piece* curPiece = m_board.findPiece(row, c);
                if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                        !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                    return true; // The king is in check
                }
            }

            // Check the y = x diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col + (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
            // Check the y = x diagonal for opponent pieces that can attack the king
            for (int r = 0; r < 8; ++r) {
                int c = col - (r - row);
                if (c >= 0 && c < 8) {
                    Piece* curPiece = m_board.findPiece(r, c);
                    if (curPiece != nullptr && isValidMove(curPlayer, curPiece->getRow(), curPiece->getCol(), row, col) &&
                            !isInLineOfSightOfKing(curPiece, curPlayer->getKing())) {
                        return true; // The king is in check
                    }
                }
            }
        }
    }

    return false; // The king is not in check
}

bool Game::isInLineOfSight(int sx, int sy, int ex, int ey) {
    int startY = sy;
    int startX = sx;
    int endY = ey;
    int endX = ex;
    // Check if the pieces are in the same row, column, or diagonal
    if (startX == endX || startY == endY || std::abs(startX - endX) == std::abs(startY - endY)) {
        // Check if there are no pieces in between
        int stepX = (startX < endX) ? 1 : ((startX > endX) ? -1 : 0);
        int stepY = (startY < endY) ? 1 : ((startY > endY) ? -1 : 0);
        int currentX = startX + stepX;
        int currentY = startY + stepY;
        while (currentX != endX || currentY != endY) {
            // Check if there is a piece in the way
            if (m_board.getBoardEntry(currentX, currentY) != nullptr) {
                return false; // There is an obstacle in the path
            }
            // Move to the next position
            currentX += stepX;
            currentY += stepY;
        }
        return true; // No obstacles found, there is line of sight
    }
    return false; // Pieces are not in the same row, column, or diagonal
}

bool Game::isInLineOfSightOfKing(Piece* startPiece, Piece * endPiece) {
    int startY = startPiece->getRow();
    int startX = startPiece->getCol();
    int endY = endPiece->getRow();
    int endX = endPiece->getCol();
    // Check if the pieces are in the same row, column, or diagonal
    if (startX == endX || startY == endY || std::abs(startX - endX) == std::abs(startY - endY)) {
        // Check if there are no pieces in between
        int stepX = (startX < endX) ? 1 : ((startX > endX) ? -1 : 0);
        int stepY = (startY < endY) ? 1 : ((startY > endY) ? -1 : 0);

        int currentX = startX + stepX;
        int currentY = startY + stepY;

        while (currentX != endX || currentY != endY) {
            // Check if there is a piece in the way
            if (m_board.getBoardEntry(currentX, currentY) != nullptr) {
                return false; // There is an obstacle in the path
            }
            // Move to the next position
            currentX += stepX;
            currentY += stepY;
        }
        return true; // No obstacles found, there is line of sight
    }
    return false; // Pieces are not in the same row, column, or diagonal
}

bool Game::isKingSurrounded(Player* curPlayer, std::pair<int, int>* unprotectedSquares) {
    Piece* curKing = curPlayer->getKing();
    if (curKing == nullptr) {
        //no king found
        return false;
    }

    int kingRow = curKing->getRow();
    int kingCol = curKing->getCol();


    int protection = 0; // counter to see how many squares are protecting the king
    int protecIndex = 0; //index to store unprotected squares

    // Check each of the 8 squares around the king
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                // Skip the center square (king's position)
                continue;
            }

            int adjacentRow = kingRow + i;
            int adjacentCol = kingCol + j;

            // Check if the adjacent square is within the board boundaries
            if (adjacentRow >= 0 && adjacentRow < 8 && adjacentCol >= 0 && adjacentCol < 8) {
                Piece* adjacentPiece = m_board.findPiece(adjacentRow, adjacentCol);
                if (adjacentPiece != nullptr) {
                    // Piece found around the king
                    protection++;
                } else {
                    // Store the unprotected square, so that line can be checked later
                    unprotectedSquares[protecIndex] = std::make_pair(adjacentCol, adjacentRow);
                    protecIndex++;
                }
            } else {
                // The adjacent square is outside the board boundaries
                protection++;
            }
        }
    }
    // Store the king position in index 8 so we know where the king is
    unprotectedSquares[protecIndex + 1] = std::make_pair(kingCol, kingRow);

    if (protection == 8) //king fully surrounded so there is no point to look further
        return true;
    else {
        return false;
    }
}

void Game::start() {
    std::queue<Player*> tempQueue = players; // Create a copy of the original queue
    std::set<Player*> playerSet;

    while (!tempQueue.empty()) {
        playerSet.insert(tempQueue.front());
        tempQueue.pop();
    }
    m_board = Board(playerSet, 0);
}
