#include "quantum_chess/Game.h"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <random>
#include <vector>

namespace quantum_chess {

Game::Game() = default;

const Board& Game::board() const {
    return board_;
}

PieceColor Game::currentTurn() const {
    return currentTurn_;
}

bool Game::parseSquare(
    const std::string& square,
    int& row,
    int& column
) {
    if (square.size() != 2) {
        return false;
    }

    const char file = static_cast<char>(
        std::tolower(static_cast<unsigned char>(square[0]))
    );

    const char rank = square[1];

    if (file < 'a' || file > 'h' ||
        rank < '1' || rank > '8') {
        return false;
    }

    column = file - 'a';
    row = 8 - (rank - '0');

    return true;
}

bool Game::makeMove(
    const std::string& source,
    const std::string& destination,
    std::string& errorMessage
) {
    int sourceRow = 0;
    int sourceColumn = 0;
    int destinationRow = 0;
    int destinationColumn = 0;

    if (!parseSquare(source, sourceRow, sourceColumn) ||
        !parseSquare(
            destination,
            destinationRow,
            destinationColumn)) {
        errorMessage = "Use squares such as e2 e4.";
        return false;
    }

    if (sourceRow == destinationRow &&
        sourceColumn == destinationColumn) {
        errorMessage =
            "Source and destination are the same.";
        return false;
    }

    const Piece sourcePiece =
        board_.at(sourceRow, sourceColumn);

    Piece destinationPiece =
        board_.at(destinationRow, destinationColumn);

    if (sourcePiece.type == PieceType::None) {
        errorMessage =
            "There is no piece on the source square.";
        return false;
    }

    if (sourcePiece.color != currentTurn_) {
        errorMessage =
            "That piece belongs to the other player.";
        return false;
    }

    if (destinationPiece.color == currentTurn_) {
        errorMessage =
            "You cannot capture your own piece.";
        return false;
    }

    if (destinationPiece.isQuantum &&
        destinationPiece.color != currentTurn_) {
        const int measuredGroupId =
            destinationPiece.quantumGroupId;

        int survivingRow = -1;
        int survivingColumn = -1;
        std::string measurementResult;

        if (!collapseQuantumGroup(
                measuredGroupId,
                survivingRow,
                survivingColumn,
                measurementResult)) {
            errorMessage =
                "Quantum measurement failed.";
            return false;
        }

        const bool targetBranchSurvived =
            survivingRow == destinationRow &&
            survivingColumn == destinationColumn;

        if (!targetBranchSurvived) {
            switchTurn();

            errorMessage =
                measurementResult +
                " The capture failed because this branch disappeared.";

            return false;
        }

        destinationPiece =
            board_.at(
                destinationRow,
                destinationColumn
            );
    }

    if (destinationPiece.type == PieceType::King) {
        errorMessage =
            "The king cannot be captured. It must be checkmated.";
        return false;
    }

    if (!isLegalMove(
            sourceRow,
            sourceColumn,
            destinationRow,
            destinationColumn,
            errorMessage)) {
        return false;
    }

    const Board previousBoard = board_;

    const bool castlingMove =
        sourcePiece.type == PieceType::King &&
        sourceRow == destinationRow &&
        std::abs(destinationColumn - sourceColumn) == 2;

    const bool enPassantCapture =
        sourcePiece.type == PieceType::Pawn &&
        destinationPiece.type == PieceType::None &&
        sourceColumn != destinationColumn &&
        enPassantAvailable_ &&
        destinationRow == enPassantTargetRow_ &&
        destinationColumn == enPassantTargetColumn_;

    const bool pawnMovedTwoSquares =
        sourcePiece.type == PieceType::Pawn &&
        std::abs(destinationRow - sourceRow) == 2;

    board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    if (enPassantCapture) {
        board_.at(
            enPassantPawnRow_,
            enPassantPawnColumn_
        ) = {};
    }

    if (castlingMove) {
        moveCastlingRook(
            destinationRow,
            sourceColumn,
            destinationColumn
        );
    }

    promotePawnIfNeeded(
        destinationRow,
        destinationColumn
    );

    if (isInCheck(currentTurn_)) {
        board_ = previousBoard;
        errorMessage =
            "That move leaves your king in check.";
        return false;
    }

    enPassantAvailable_ = false;
    enPassantTargetRow_ = -1;
    enPassantTargetColumn_ = -1;
    enPassantPawnRow_ = -1;
    enPassantPawnColumn_ = -1;

    if (pawnMovedTwoSquares) {
        enPassantAvailable_ = true;
        enPassantTargetRow_ =
            (sourceRow + destinationRow) / 2;
        enPassantTargetColumn_ =
            destinationColumn;
        enPassantPawnRow_ =
            destinationRow;
        enPassantPawnColumn_ =
            destinationColumn;
    }

    switchTurn();
    errorMessage.clear();

    return true;
}


bool Game::splitKnight(
    const std::string& source,
    const std::string& firstDestination,
    const std::string& secondDestination,
    std::string& errorMessage
) {
    int sourceRow = 0;
    int sourceColumn = 0;
    int firstRow = 0;
    int firstColumn = 0;
    int secondRow = 0;
    int secondColumn = 0;

    if (!parseSquare(source, sourceRow, sourceColumn) ||
        !parseSquare(
            firstDestination,
            firstRow,
            firstColumn
        ) ||
        !parseSquare(
            secondDestination,
            secondRow,
            secondColumn
        )) {
        errorMessage =
            "Use: split b1 a3 c3";
        return false;
    }

    if (firstRow == secondRow &&
        firstColumn == secondColumn) {
        errorMessage =
            "The two quantum destinations must be different.";
        return false;
    }

    const Piece sourcePiece =
        board_.at(sourceRow, sourceColumn);

    if (sourcePiece.type != PieceType::Knight) {
        errorMessage =
            "Only knights can split in this version.";
        return false;
    }

    if (sourcePiece.color != currentTurn_) {
        errorMessage =
            "That knight belongs to the other player.";
        return false;
    }

    if (sourcePiece.isQuantum) {
        errorMessage =
            "A quantum knight cannot split again.";
        return false;
    }

    if (board_.at(firstRow, firstColumn).type !=
            PieceType::None ||
        board_.at(secondRow, secondColumn).type !=
            PieceType::None) {
        errorMessage =
            "Both quantum destination squares must be empty.";
        return false;
    }

    const int firstRowDifference =
        std::abs(firstRow - sourceRow);

    const int firstColumnDifference =
        std::abs(firstColumn - sourceColumn);

    const int secondRowDifference =
        std::abs(secondRow - sourceRow);

    const int secondColumnDifference =
        std::abs(secondColumn - sourceColumn);

    const bool firstMoveIsLegal =
        (firstRowDifference == 2 &&
         firstColumnDifference == 1) ||
        (firstRowDifference == 1 &&
         firstColumnDifference == 2);

    const bool secondMoveIsLegal =
        (secondRowDifference == 2 &&
         secondColumnDifference == 1) ||
        (secondRowDifference == 1 &&
         secondColumnDifference == 2);

    if (!firstMoveIsLegal || !secondMoveIsLegal) {
        errorMessage =
            "Both destinations must be legal knight moves.";
        return false;
    }

    Piece firstBranch = sourcePiece;
    Piece secondBranch = sourcePiece;

    const int groupId = nextQuantumGroupId_++;

    firstBranch.hasMoved = true;
    firstBranch.isQuantum = true;
    firstBranch.quantumGroupId = groupId;
    firstBranch.probability = 0.5;

    secondBranch.hasMoved = true;
    secondBranch.isQuantum = true;
    secondBranch.quantumGroupId = groupId;
    secondBranch.probability = 0.5;

    Board previousBoard = board_;

    board_.clearSquare(
        sourceRow,
        sourceColumn
    );

    board_.setPiece(
        firstRow,
        firstColumn,
        firstBranch
    );

    board_.setPiece(
        secondRow,
        secondColumn,
        secondBranch
    );

    if (isInCheck(currentTurn_)) {
        board_ = previousBoard;
        --nextQuantumGroupId_;

        errorMessage =
            "The split would leave your king in check.";
        return false;
    }

    switchTurn();
    errorMessage.clear();

    return true;
}



bool Game::collapseQuantumGroup(
    int quantumGroupId,
    int& survivingRow,
    int& survivingColumn,
    std::string& resultMessage
) {
    struct Branch {
        int row;
        int column;
        double probability;
    };

    std::vector<Branch> branches;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (piece.isQuantum &&
                piece.quantumGroupId == quantumGroupId) {
                branches.push_back({
                    row,
                    column,
                    piece.probability
                });
            }
        }
    }

    if (branches.empty()) {
        return false;
    }

    double totalProbability = 0.0;

    for (const Branch& branch : branches) {
        totalProbability += branch.probability;
    }

    if (totalProbability <= 0.0) {
        return false;
    }

    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());

    std::uniform_real_distribution<double> distribution(
        0.0,
        totalProbability
    );

    const double measuredValue =
        distribution(generator);

    double accumulatedProbability = 0.0;
    std::size_t selectedIndex =
        branches.size() - 1;

    for (std::size_t index = 0;
         index < branches.size();
         ++index) {
        accumulatedProbability +=
            branches[index].probability;

        if (measuredValue <= accumulatedProbability) {
            selectedIndex = index;
            break;
        }
    }

    Piece survivingPiece =
        board_.at(
            branches[selectedIndex].row,
            branches[selectedIndex].column
        );

    for (const Branch& branch : branches) {
        board_.clearSquare(
            branch.row,
            branch.column
        );
    }

    survivingPiece.isQuantum = false;
    survivingPiece.quantumGroupId = -1;
    survivingPiece.probability = 1.0;

    survivingRow =
        branches[selectedIndex].row;

    survivingColumn =
        branches[selectedIndex].column;

    board_.setPiece(
        survivingRow,
        survivingColumn,
        survivingPiece
    );

    const char file =
        static_cast<char>('a' + survivingColumn);

    const int rank =
        8 - survivingRow;

    std::ostringstream output;

    output
        << "Quantum group "
        << quantumGroupId
        << " collapsed to "
        << file
        << rank
        << ".";

    resultMessage = output.str();
    return true;
}

bool Game::measureQuantumPiece(
    const std::string& square,
    std::string& resultMessage,
    std::string& errorMessage
) {
    int row = 0;
    int column = 0;

    if (!parseSquare(square, row, column)) {
        errorMessage =
            "Use a valid square such as a3.";
        return false;
    }

    const Piece selectedPiece =
        board_.at(row, column);

    if (!selectedPiece.isQuantum ||
        selectedPiece.quantumGroupId < 0) {
        errorMessage =
            "There is no quantum piece on that square.";
        return false;
    }

    int survivingRow = -1;
    int survivingColumn = -1;

    if (!collapseQuantumGroup(
            selectedPiece.quantumGroupId,
            survivingRow,
            survivingColumn,
            resultMessage)) {
        errorMessage =
            "Quantum measurement failed.";
        return false;
    }

    errorMessage.clear();
    return true;
}

std::string Game::quantumSummary() const {
    std::ostringstream output;
    bool foundQuantumPiece = false;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (!piece.isQuantum) {
                continue;
            }

            foundQuantumPiece = true;

            const char file =
                static_cast<char>('a' + column);

            const int rank = 8 - row;

            output
                << "Quantum group "
                << piece.quantumGroupId
                << ": knight at "
                << file
                << rank
                << " with "
                << static_cast<int>(
                    piece.probability * 100.0
                )
                << "% probability\n";
        }
    }

    if (!foundQuantumPiece) {
        return "";
    }

    return output.str();
}

bool Game::isLegalMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& piece =
        board_.at(sourceRow, sourceColumn);

    const int rowDifference =
        destinationRow - sourceRow;

    const int columnDifference =
        destinationColumn - sourceColumn;

    const int absoluteRowDifference =
        std::abs(rowDifference);

    const int absoluteColumnDifference =
        std::abs(columnDifference);

    switch (piece.type) {
        case PieceType::Pawn:
            return isLegalPawnMove(
                sourceRow,
                sourceColumn,
                destinationRow,
                destinationColumn,
                errorMessage
            );

        case PieceType::Knight:
            if ((absoluteRowDifference == 2 &&
                 absoluteColumnDifference == 1) ||
                (absoluteRowDifference == 1 &&
                 absoluteColumnDifference == 2)) {
                return true;
            }

            errorMessage =
                "A knight must move in an L shape.";
            return false;

        case PieceType::Bishop:
            if (absoluteRowDifference !=
                absoluteColumnDifference) {
                errorMessage =
                    "A bishop must move diagonally.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The bishop's path is blocked.";
                return false;
            }

            return true;

        case PieceType::Rook:
            if (rowDifference != 0 &&
                columnDifference != 0) {
                errorMessage =
                    "A rook must move horizontally or vertically.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The rook's path is blocked.";
                return false;
            }

            return true;

        case PieceType::Queen:
            if (rowDifference != 0 &&
                columnDifference != 0 &&
                absoluteRowDifference !=
                    absoluteColumnDifference) {
                errorMessage =
                    "A queen must move straight or diagonally.";
                return false;
            }

            if (!board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn)) {
                errorMessage =
                    "The queen's path is blocked.";
                return false;
            }

            return true;

        case PieceType::King:
            if (absoluteRowDifference == 0 &&
                absoluteColumnDifference == 2) {
                return isLegalCastlingMove(
                    sourceRow,
                    sourceColumn,
                    destinationRow,
                    destinationColumn,
                    errorMessage
                );
            }

            if (absoluteRowDifference <= 1 &&
                absoluteColumnDifference <= 1) {
                return true;
            }

            errorMessage =
                "A king can move one square or castle.";
            return false;

        case PieceType::None:
            errorMessage =
                "There is no piece on the source square.";
            return false;
    }

    errorMessage = "Unknown piece type.";
    return false;
}

bool Game::isLegalPawnMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& pawn =
        board_.at(sourceRow, sourceColumn);

    const Piece& destination =
        board_.at(destinationRow, destinationColumn);

    const int direction =
        pawn.color == PieceColor::White ? -1 : 1;

    const int startingRow =
        pawn.color == PieceColor::White ? 6 : 1;

    const int rowDifference =
        destinationRow - sourceRow;

    const int columnDifference =
        destinationColumn - sourceColumn;

    if (columnDifference == 0) {
        if (destination.type != PieceType::None) {
            errorMessage =
                "A pawn cannot move forward into an occupied square.";
            return false;
        }

        if (rowDifference == direction) {
            return true;
        }

        if (sourceRow == startingRow &&
            rowDifference == 2 * direction) {
            const int middleRow =
                sourceRow + direction;

            if (board_.at(
                    middleRow,
                    sourceColumn).type ==
                PieceType::None) {
                return true;
            }

            errorMessage = "The pawn is blocked.";
            return false;
        }

        errorMessage = "Illegal pawn movement.";
        return false;
    }

    if (std::abs(columnDifference) == 1 &&
        rowDifference == direction) {
        if (destination.type != PieceType::None) {
            return true;
        }

        if (enPassantAvailable_ &&
            destinationRow == enPassantTargetRow_ &&
            destinationColumn == enPassantTargetColumn_) {
            const Piece& capturedPawn =
                board_.at(
                    enPassantPawnRow_,
                    enPassantPawnColumn_
                );

            if (capturedPawn.type == PieceType::Pawn &&
                capturedPawn.color != pawn.color) {
                return true;
            }
        }

        errorMessage =
            "A pawn can move diagonally only when capturing.";
        return false;
    }

    errorMessage = "Illegal pawn movement.";
    return false;
}

bool Game::pieceAttacksSquare(
    int sourceRow,
    int sourceColumn,
    int targetRow,
    int targetColumn
) const {
    const Piece& piece =
        board_.at(sourceRow, sourceColumn);

    if (piece.type == PieceType::None) {
        return false;
    }

    const int rowDifference =
        targetRow - sourceRow;

    const int columnDifference =
        targetColumn - sourceColumn;

    const int absoluteRowDifference =
        std::abs(rowDifference);

    const int absoluteColumnDifference =
        std::abs(columnDifference);

    switch (piece.type) {
        case PieceType::Pawn: {
            const int direction =
                piece.color == PieceColor::White
                    ? -1
                    : 1;

            return rowDifference == direction &&
                   absoluteColumnDifference == 1;
        }

        case PieceType::Knight:
            return
                (absoluteRowDifference == 2 &&
                 absoluteColumnDifference == 1) ||
                (absoluteRowDifference == 1 &&
                 absoluteColumnDifference == 2);

        case PieceType::Bishop:
            return
                absoluteRowDifference ==
                    absoluteColumnDifference &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::Rook:
            return
                (rowDifference == 0 ||
                 columnDifference == 0) &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::Queen:
            return
                (rowDifference == 0 ||
                 columnDifference == 0 ||
                 absoluteRowDifference ==
                    absoluteColumnDifference) &&
                board_.isPathClear(
                    sourceRow,
                    sourceColumn,
                    targetRow,
                    targetColumn
                );

        case PieceType::King:
            return
                absoluteRowDifference <= 1 &&
                absoluteColumnDifference <= 1;

        case PieceType::None:
            return false;
    }

    return false;
}

bool Game::isInCheck(PieceColor color) const {
    int kingRow = -1;
    int kingColumn = -1;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (piece.type == PieceType::King &&
                piece.color == color) {
                kingRow = row;
                kingColumn = column;
            }
        }
    }

    if (kingRow == -1 || kingColumn == -1) {
        return false;
    }

    const PieceColor opponent =
        color == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;

    for (int row = 0; row < 8; ++row) {
        for (int column = 0; column < 8; ++column) {
            const Piece& piece =
                board_.at(row, column);

            if (piece.color == opponent &&
                pieceAttacksSquare(
                    row,
                    column,
                    kingRow,
                    kingColumn)) {
                return true;
            }
        }
    }

    return false;
}



bool Game::isLegalCastlingMove(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    std::string& errorMessage
) const {
    const Piece& king =
        board_.at(sourceRow, sourceColumn);

    const int homeRow =
        king.color == PieceColor::White ? 7 : 0;

    if (sourceRow != homeRow ||
        destinationRow != homeRow ||
        sourceColumn != 4) {
        errorMessage =
            "The king is not on its castling square.";
        return false;
    }

    if (king.hasMoved) {
        errorMessage =
            "The king has already moved.";
        return false;
    }

    const bool kingSide =
        destinationColumn == 6;

    const bool queenSide =
        destinationColumn == 2;

    if (!kingSide && !queenSide) {
        errorMessage =
            "Invalid castling destination.";
        return false;
    }

    const int rookColumn =
        kingSide ? 7 : 0;

    const Piece& rook =
        board_.at(homeRow, rookColumn);

    if (rook.type != PieceType::Rook ||
        rook.color != king.color) {
        errorMessage =
            "The required rook is not available.";
        return false;
    }

    if (rook.hasMoved) {
        errorMessage =
            "The rook has already moved.";
        return false;
    }

    if (!board_.isPathClear(
            homeRow,
            sourceColumn,
            homeRow,
            rookColumn)) {
        errorMessage =
            "Pieces are blocking the castling path.";
        return false;
    }

    if (isInCheck(king.color)) {
        errorMessage =
            "You cannot castle while in check.";
        return false;
    }

    const int direction =
        kingSide ? 1 : -1;

    Game transitPosition = *this;

    transitPosition.board_.movePiece(
        sourceRow,
        sourceColumn,
        sourceRow,
        sourceColumn + direction
    );

    if (transitPosition.isInCheck(king.color)) {
        errorMessage =
            "The king cannot castle through check.";
        return false;
    }

    Game destinationPosition = *this;

    destinationPosition.board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    if (destinationPosition.isInCheck(king.color)) {
        errorMessage =
            "The king cannot castle into check.";
        return false;
    }

    return true;
}

void Game::moveCastlingRook(
    int kingRow,
    int kingSourceColumn,
    int kingDestinationColumn
) {
    if (kingSourceColumn != 4) {
        return;
    }

    if (kingDestinationColumn == 6) {
        board_.movePiece(
            kingRow,
            7,
            kingRow,
            5
        );
    } else if (kingDestinationColumn == 2) {
        board_.movePiece(
            kingRow,
            0,
            kingRow,
            3
        );
    }
}

bool Game::moveLeavesKingInCheck(
    int sourceRow,
    int sourceColumn,
    int destinationRow,
    int destinationColumn,
    PieceColor color
) const {
    Game testGame = *this;

    const Piece movingPiece =
        testGame.board_.at(sourceRow, sourceColumn);

    const Piece destinationPiece =
        testGame.board_.at(
            destinationRow,
            destinationColumn
        );

    const bool castlingMove =
        movingPiece.type == PieceType::King &&
        sourceRow == destinationRow &&
        std::abs(destinationColumn - sourceColumn) == 2;

    const bool enPassantCapture =
        movingPiece.type == PieceType::Pawn &&
        destinationPiece.type == PieceType::None &&
        sourceColumn != destinationColumn &&
        testGame.enPassantAvailable_ &&
        destinationRow ==
            testGame.enPassantTargetRow_ &&
        destinationColumn ==
            testGame.enPassantTargetColumn_;

    testGame.board_.movePiece(
        sourceRow,
        sourceColumn,
        destinationRow,
        destinationColumn
    );

    if (enPassantCapture) {
        testGame.board_.at(
            testGame.enPassantPawnRow_,
            testGame.enPassantPawnColumn_
        ) = {};
    }

    if (castlingMove) {
        testGame.moveCastlingRook(
            destinationRow,
            sourceColumn,
            destinationColumn
        );
    }

    testGame.promotePawnIfNeeded(
        destinationRow,
        destinationColumn
    );

    return testGame.isInCheck(color);
}

bool Game::hasAnyLegalMove(PieceColor color) const {
    for (int sourceRow = 0; sourceRow < 8; ++sourceRow) {
        for (int sourceColumn = 0; sourceColumn < 8; ++sourceColumn) {
            const Piece& piece =
                board_.at(sourceRow, sourceColumn);

            if (piece.color != color) {
                continue;
            }

            for (int destinationRow = 0;
                 destinationRow < 8;
                 ++destinationRow) {
                for (int destinationColumn = 0;
                     destinationColumn < 8;
                     ++destinationColumn) {

                    if (sourceRow == destinationRow &&
                        sourceColumn == destinationColumn) {
                        continue;
                    }

                    const Piece& destination =
                        board_.at(
                            destinationRow,
                            destinationColumn
                        );

                    if (destination.color == color) {
                        continue;
                    }

                    if (destination.type == PieceType::King) {
                        continue;
                    }

                    std::string errorMessage;

                    if (!isLegalMove(
                            sourceRow,
                            sourceColumn,
                            destinationRow,
                            destinationColumn,
                            errorMessage)) {
                        continue;
                    }

                    if (!moveLeavesKingInCheck(
                            sourceRow,
                            sourceColumn,
                            destinationRow,
                            destinationColumn,
                            color)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Game::isCheckmate(PieceColor color) const {
    return isInCheck(color) &&
           !hasAnyLegalMove(color);
}

bool Game::isStalemate(PieceColor color) const {
    return !isInCheck(color) &&
           !hasAnyLegalMove(color);
}

void Game::promotePawnIfNeeded(
    int row,
    int column
) {
    Piece& piece = board_.at(row, column);

    if (piece.type != PieceType::Pawn) {
        return;
    }

    const bool whitePromotion =
        piece.color == PieceColor::White &&
        row == 0;

    const bool blackPromotion =
        piece.color == PieceColor::Black &&
        row == 7;

    if (whitePromotion || blackPromotion) {
        piece.type = PieceType::Queen;
    }
}

void Game::switchTurn() {
    currentTurn_ =
        currentTurn_ == PieceColor::White
            ? PieceColor::Black
            : PieceColor::White;
}

}
