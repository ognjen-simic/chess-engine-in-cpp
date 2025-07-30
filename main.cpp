#include <bitset>
#include <iostream>
#include <string>
#include <map>
#include "uci.h"
#include <vector>
#include <limits>
#include "Board.h"
#include <algorithm>
#include <chrono>
#include "zobrist.h"
#include "tt.h"

void printFullBoard(const Board& board)
{
    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            int index = row * 8 + col;
            char piece = '.';

            if (board.whitePawns[index]) piece = 'P';
            else if (board.blackPawns[index]) piece = 'p';
            else if (board.whiteRooks[index]) piece = 'R';
            else if (board.blackRooks[index]) piece = 'r';
            else if (board.whiteKnights[index]) piece = 'N';
            else if (board.blackKnights[index]) piece = 'n';
            else if (board.whiteBishops[index]) piece = 'B';
            else if (board.blackBishops[index]) piece = 'b';
            else if (board.whiteQueen[index]) piece = 'Q';
            else if (board.blackQueen[index]) piece = 'q';
            else if (board.whiteKing[index]) piece = 'K';
            else if (board.blackKing[index]) piece = 'k';

            std::cout << piece;
        }
        std::cout << '\n';
    }
}

std::bitset<64> generateKnightMoves(std::bitset<64> knights)
 {
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notABFile = ~std::bitset<64>(0x0303030303030303ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);
    std::bitset<64> notGHFile = ~std::bitset<64>(0xC0C0C0C0C0C0C0C0ULL);

    std::bitset<64> result;

    result |= (knights & notAFile) << 15;  // 2 up 1 left
    result |= (knights & notHFile) << 17;  // 2 up 1 right
    result |= (knights & notABFile) << 6;  // 1 up 2 left
    result |= (knights & notGHFile) << 10;  // 1 up 2 right

    result |= (knights & notAFile) >> 17;  // 2 down 1 left
    result |= (knights & notHFile) >> 15;  // 2 down 1 right
    result |= (knights & notABFile) >> 10;  // 1 down 2 left
    result |= (knights & notGHFile) >> 6;  // 1 down 2 right

    return result;
 }

std::bitset<64> generateWhitePawnCaptures(std::bitset<64> whitePawns, std::bitset<64> blackPieces)
{
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (whitePawns & notAFile) << 7;
    std::bitset<64> capturesRight = (whitePawns & notHFile) << 9;

    return (capturesLeft | capturesRight) & blackPieces;
}

std::bitset<64> generateBlackPawnCaptures(std::bitset<64> blackPawns, std::bitset<64> whitePieces)
{
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (blackPawns & notHFile) >> 7;
    std::bitset<64> capturesRight = (blackPawns & notAFile) >> 9;

    return (capturesLeft | capturesRight) & whitePieces;
}

std::bitset<64> generateWhitePawnMoves(std::bitset<64> whitePawns, std::bitset<64> blackPieces, std::bitset<64> allPieces)
 {
    std::bitset<64> rank2 = (0x000000000000FF00ULL);

    std::bitset<64> oneStep = (whitePawns << 8) & ~allPieces;
    std::bitset<64> twoStep = ((whitePawns & rank2) << 8) & ~allPieces;
    twoStep = (twoStep << 8) & ~allPieces;

    std::bitset<64> captures = generateWhitePawnCaptures(whitePawns, blackPieces);

    return oneStep | twoStep | captures;
 }

std::bitset<64> generateBlackPawnMoves(std::bitset<64> blackPawns, std::bitset<64> whitePieces, std::bitset<64> allPieces)
 {
    std::bitset<64> rank7 = (0x00FF000000000000ULL);

    std::bitset<64> oneStep = (blackPawns >> 8) & ~allPieces;
    std::bitset<64> twoStep = ((blackPawns & rank7) >> 8) & ~allPieces;
    twoStep = (twoStep >> 8) & ~allPieces;

    std::bitset<64> captures = generateBlackPawnCaptures(blackPawns, whitePieces);

    return oneStep | twoStep | captures;
 }

std::bitset<64> constWhitePawnCaptures(std::bitset<64> whitePawns)
{
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (whitePawns & notAFile) << 7;
    std::bitset<64> capturesRight = (whitePawns & notHFile) << 9;

    return (capturesLeft | capturesRight);
}

std::bitset<64> constBlackPawnCaptures(std::bitset<64> blackPawns)
{
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (blackPawns & notHFile) >> 7;
    std::bitset<64> capturesRight = (blackPawns & notAFile) >> 9;

    return (capturesLeft | capturesRight);
}

std::bitset<64> generateRookMoves(int square, std::bitset<64> ownPieces, std::bitset<64> allPieces)
{
    std::bitset<64> moves;
    int directions[4] = {8, -8, 1, -1};

    for (int d = 0; d < 4; ++d)
    {
        int sq = square;
        while (true)
        {
            int nextSq = sq + directions[d];

            if (directions[d] == 1 && nextSq % 8 == 0) break;
            if (directions[d] == -1 && sq % 8 == 0) break;

            if (nextSq < 0 || nextSq >= 64) break;
            if (ownPieces[nextSq]) break;
            moves.set(nextSq);
            if (allPieces[nextSq]) break;

            sq = nextSq;
        }
    }
    return moves;
}

std::bitset<64> generateBishopMoves(int square, std::bitset<64> ownPieces, std::bitset<64> allPieces)
{
    std::bitset<64> moves;
    int directions[4] = {9, 7, -9, -7};

    for (int d = 0; d < 4; ++d)
    {
        int sq = square;
        while (true)
        {
            int nextSq = sq + directions[d];

            if ((directions[d] == 9 || directions[d] == -7) && nextSq % 8 == 0) break;
            if ((directions[d] == -9 || directions[d] == 7) && sq % 8 == 0) break;

            if (nextSq < 0 || nextSq >= 64) break;
            if (ownPieces[nextSq]) break;
            moves.set(nextSq);
            if (allPieces[nextSq]) break;

            sq = nextSq;
        }
    }
    return moves;
}

std::bitset<64> generateQueenMoves(int square, std::bitset<64> ownPieces, std::bitset<64> allPieces)
{
    return (generateRookMoves(square, ownPieces, allPieces) | generateBishopMoves(square, ownPieces, allPieces));
}

std::bitset<64> generateKingMoves(int square, std::bitset<64> ownPieces, std::bitset<64> allPieces)
{
    std::bitset<64> moves;
    int directions[8] = {1, -1, 8, -8, 9, -9, 7, -7};

    for (int d = 0; d < 8; ++d)
    {
        int nextSq = square + directions[d];

        if (nextSq < 0 || nextSq >= 64) continue;

        int fromFile = square % 8;
        int toFile = nextSq % 8;
        if (std::abs(fromFile - toFile) > 1) continue;

        if (ownPieces[nextSq]) continue;

        moves.set(nextSq);
    }
    return moves;
}

int notationToIndex(const std::string& notation)
 {
    if (notation.length() != 2) return -1;

    char file = notation[0];
    char rank = notation[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') return -1;

    int fileIndex = file - 'a';
    int rankIndex = rank - '1';

    return rankIndex * 8 + fileIndex;
 }

std::string indexToNotation(int index)
{
    if (index < 0 || index >= 64) return "";

    char file = 'a' + (index % 8);
    char rank = '1' + (index / 8);

    return std::string (1, file) + rank;
}

std::bitset<64> generateWhitePawnAttacks(const Board& board)
{
     std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (board.whitePawns & notAFile) << 7;
    std::bitset<64> capturesRight = (board.whitePawns & notHFile) << 9;

    return (capturesLeft | capturesRight);
}

std::bitset<64> generateBlackPawnAttacks(const Board& board)
{
     std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);

    std::bitset<64> capturesLeft = (board.blackPawns & notAFile) >> 7;
    std::bitset<64> capturesRight = (board.blackPawns & notHFile) >> 9;

    return (capturesLeft | capturesRight);
}

std::bitset<64> generateWhiteAttacks(const Board& board)
{
    std::bitset<64> attacks;

    for (int sq = 0; sq < 64; ++sq)
    {
        if (board.whiteKnights[sq])
        {
            attacks |= generateKnightMoves(std::bitset<64> (1ULL << sq));
        }
    }

    attacks |= generateWhitePawnAttacks(board);

    for (int sq = 0; sq < 64; ++sq)
    {
        if (board.whiteBishops[sq])
            attacks |= generateBishopMoves(sq, board.getOwnPieces(true), board.getAllPieces());
        else if (board.whiteRooks[sq])
            attacks |= generateRookMoves(sq, board.getOwnPieces(true), board.getAllPieces());
        else if (board.whiteQueen[sq])
            attacks |= generateQueenMoves(sq, board.getOwnPieces(true), board.getAllPieces());
        else if (board.whiteKing[sq])
            attacks |= generateKingMoves(sq, board.getOwnPieces(true), board.getAllPieces());
    }

    return attacks;
}

std::bitset<64> generateBlackAttacks(const Board& board)
{
    std::bitset<64> attacks;

    for (int sq = 0; sq < 64; ++sq)
    {
        if (board.blackKnights[sq])
        {
            attacks |= generateKnightMoves(std::bitset<64> (1ULL << sq));
        }
    }

    attacks |= generateBlackPawnAttacks(board);

    for (int sq = 0; sq < 64; ++sq)
    {
        if (board.blackBishops[sq])
            attacks |= generateBishopMoves(sq, board.getOwnPieces(false), board.getAllPieces());
        else if (board.blackRooks[sq])
            attacks |= generateRookMoves(sq, board.getOwnPieces(false), board.getAllPieces());
        else if (board.blackQueen[sq])
            attacks |= generateQueenMoves(sq, board.getOwnPieces(false), board.getAllPieces());
        else if (board.blackKing[sq])
            attacks |= generateKingMoves(sq, board.getOwnPieces(false), board.getAllPieces());
    }

    return attacks;
}

std::map<int, std::bitset<64>> getPinnedPieces(const Board& board)
    {
    int kingSquare = (board.whiteToMove ? board.whiteKing : board.blackKing)._Find_first();

    if (kingSquare >= 64) {
        std::cerr << "Error: King not found on board!" << std::endl;
        return std::map<int, std::bitset<64>>();  // Return empty map
    }

    std::bitset<64> enemyBishops = board.whiteToMove ? board.blackBishops : board.whiteBishops;
    std::bitset<64> enemyRooks = board.whiteToMove ? board.blackRooks : board.whiteRooks;
    std::bitset<64> enemyQueens = board.whiteToMove ? board.blackQueen : board.whiteQueen;

    std::bitset<64> ownPieces = board.whiteToMove ? board.getWhitePieces() : board.getBlackPieces();
    std::bitset<64> allPieces    = board.getAllPieces();

    std::bitset<64> pinnedPieces;
    std::map<int, std::bitset<64>> pinnedLines;

    int directions[8] = {1, -1, 8, -8, 7, -7, 9, -9};

    for (int d = 0; d < 8; ++d)
    {
        int dir = directions[d];
        int sq = kingSquare;
        bool foundOwn = false;
        int potentialPinned = -1;

        std::bitset<64> potentialLine;

        while (true)
        {
            sq += dir;

            if (sq < 0 || sq >= 64) break;

            int prevFile = (sq - dir) % 8;
            int currFile = sq % 8;
            if (abs(currFile - prevFile) > 1) break;

            potentialLine.set(sq);

            if (ownPieces[sq])
            {
                if (!foundOwn)
                {
                    foundOwn = true;
                    potentialPinned = sq;
                }
                else break; 
            }
            else if (enemyQueens[sq] ||
                     (enemyBishops[sq] && (dir == 7 || dir == -7 || dir == 9 || dir == -9)) ||
                     (enemyRooks[sq] && (dir == 1 || dir == -1 || dir == 8 || dir == -8)))
            {
                if (foundOwn && potentialPinned != -1)
                {
                    potentialLine.reset(potentialPinned);
                    pinnedLines[potentialPinned] = potentialLine;
                }
                break;
            }
            else if (allPieces[sq])
            {
                break;
            }
        }
    }

    return pinnedLines;
}

bool isKingInCheck(const Board& board)
{
    int kingIndex = board.whiteToMove ? board.whiteKing._Find_first() : board.blackKing._Find_first();

    std::bitset<64> attackMap = board.whiteToMove ?
        generateBlackAttacks(board)
        :
        generateWhiteAttacks(board);

    return attackMap[kingIndex];
}

bool makeMove(std::string move, Board& board)
{
    if (move.length() != 4 && move.length() != 5) return false;

    int from = notationToIndex(move.substr(0, 2));
    int to = notationToIndex(move.substr(2, 2));
    if (from == -1 || to == -1) return false;

    if (!(board.getAllPieces()[from])) return false;

    bool isPromotion = move.length() == 5;
    char promotionPiece = isPromotion ? move[4] : '\0';

    std::map<int, std::bitset<64>> pinnedLines = getPinnedPieces(board);

if (pinnedLines.count(from))
{
    if (!pinnedLines[from][to])
        return false;
}

    if (board.whiteToMove)
    {
        if (board.whiteKnights[from])
        {
            board.en_passant = -1;

            std::bitset<64> thisKnight(1ULL << from);
            std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
            if (!legalMoves[to]) return false;
            if (board.getWhitePieces()[to]) return false;

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whiteKnights.reset(from);
            newBoard.whiteKnights.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard; 
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.whitePawns[from])
        {
            if (to == board.en_passant && to / 8 == 5)
            {
                Board newBoard = board;
                
                newBoard.blackPawns.reset(to - 8);
                newBoard.whitePawns.reset(from);
                newBoard.whitePawns.set(to);

                if (isKingInCheck(newBoard)) return false;
                board = newBoard;
                board.whiteToMove = !board.whiteToMove;
                board.hash = Zobrist::computeHash(board);
                return true;
            }
            else board.en_passant = -1;

            if (to / 8 == 7 && move.length() != 5) return false;
            else if (to / 8 == 7 && move.length() ==5)
            {
                if (move[4] != 'q' && move[4] != 'r' && move[4] != 'b' && move[4] != 'n') return false;
            }

            std::bitset<64> thisPawn(1ULL << from);
            std::bitset<64> legalMoves = generateWhitePawnMoves(thisPawn, board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (!(board.getAllPieces()[from])) return false;

            if (from / 8 == 1 && to / 8 == 3)
            {
                board.en_passant = from + 8;
            }
            else
            {
                board.en_passant = -1;
            }

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whitePawns.reset(from);
            if (isPromotion && to / 8 == 7)
            {
                switch (promotionPiece)
                {
                    case 'q': newBoard.whiteQueen.set(to); break;
                    case 'r': newBoard.whiteRooks.set(to); break;
                    case 'b': newBoard.whiteBishops.set(to); break;
                    case 'n': newBoard.whiteKnights.set(to); break;
                    default: return false;
                }
            }
            else
            {
                newBoard.whitePawns.set(to);
            }

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.whiteRooks[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateRookMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getWhitePieces()[to]) return false;
            if (from == notationToIndex("a1")) board.whiteCanCastleQueenside = false;
            else if (from == notationToIndex("h1")) board.whiteCanCastleKingside = false;

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whiteRooks.reset(from);
            newBoard.whiteRooks.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.whiteBishops[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateBishopMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getWhitePieces()[to]) return false;

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whiteBishops.reset(from);
            newBoard.whiteBishops.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.whiteQueen[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateQueenMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getWhitePieces()[to]) return false;

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whiteQueen.reset(from);
            newBoard.whiteQueen.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.whiteKing[from])
        {
            board.en_passant = -1;

            if (to == notationToIndex("g1") && board.whiteCanCastleKingside)
            {
                if (!board.getAllPieces()[notationToIndex("f1")] && !board.getAllPieces()[notationToIndex("g1")])
                {
                    Board newBoard = board;

                    newBoard.whiteKing.reset(from);
                    newBoard.whiteKing.set(to);
                    newBoard.whiteRooks.reset(notationToIndex("h1"));
                    newBoard.whiteRooks.set(notationToIndex("f1"));

                    if (isKingInCheck(newBoard) || generateBlackAttacks(newBoard)[notationToIndex("f1")]) return false;
                    newBoard.whiteCanCastleKingside = false;
                    newBoard.whiteCanCastleQueenside = false;
                    newBoard.whiteCastled = true;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.hash = Zobrist::computeHash(board);
                    return true;
                }
            }
            else if (to == notationToIndex("c1") && board.whiteCanCastleQueenside)
            {
                if (!board.getAllPieces()[notationToIndex("b1")] && !board.getAllPieces()[notationToIndex("c1")] && !board.getAllPieces()[notationToIndex("d1")])
                {
                    Board newBoard = board;

                    newBoard.whiteKing.reset(from);
                    newBoard.whiteKing.set(to);
                    newBoard.whiteRooks.reset(notationToIndex("a1"));
                    newBoard.whiteRooks.set(notationToIndex("d1"));

                    if (isKingInCheck(newBoard) || generateBlackAttacks(newBoard)[notationToIndex("d1")]) return false;
                    newBoard.whiteCanCastleKingside = false;
                    newBoard.whiteCanCastleQueenside = false;
                    newBoard.whiteCastled =true;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.hash = Zobrist::computeHash(board);
                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, board.getWhitePieces(), board.getAllPieces()) & ~generateBlackAttacks(board);
            if (!legalMoves[to]) return false;
            if (board.getWhitePieces()[to]) return false;

            Board newBoard = board;

            newBoard.blackKnights.reset(to);
            newBoard.blackPawns.reset(to);
            newBoard.blackRooks.reset(to);
            newBoard.blackBishops.reset(to);
            newBoard.blackQueen.reset(to);
            newBoard.blackKing.reset(to);

            newBoard.whiteKing.reset(from);
            newBoard.whiteKing.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;

            board.whiteCanCastleKingside = false;
            board.whiteCanCastleQueenside = false;

            board.hash = Zobrist::computeHash(board);

            return true;
        }
    }
    
    else 
    {
        if (board.blackKnights[from])
        {
            board.en_passant = -1;

            std::bitset<64> thisKnight(1ULL << from);
            std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;

            Board newBoard = board;

            newBoard.whiteKnights.reset(to);
            newBoard.whitePawns.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);

            newBoard.blackKnights.reset(from);
            newBoard.blackKnights.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.blackPawns[from])
        {
            board.en_passant = -1;

            if (to == board.en_passant && to / 8 == 2)
            {
                Board newBoard = board;
        
                newBoard.whitePawns.reset(to + 8);
                newBoard.blackPawns.reset(from);
                newBoard.blackPawns.set(to);

                if (isKingInCheck(newBoard)) return false;
                board = newBoard;
                board.whiteToMove = !board.whiteToMove;
                board.hash = Zobrist::computeHash(board);
                return true;
            }
            else board.en_passant = -1;

            if (to / 8 == 0 && move.length() != 5) return false;
            else if (to / 8 == 0 && move.length() ==5)
            {
                if (move[4] != 'q' && move[4] != 'r' && move[4] != 'b' && move[4] != 'n') return false;
            }

            std::bitset<64> thisPawn(1ULL << from);
            std::bitset<64> legalMoves = generateBlackPawnMoves(thisPawn, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;

            if (from / 8 == 6 && to / 8 == 4)
            {
                board.en_passant = from - 8;
            }
            else
            {
                board.en_passant = -1;
            }

            Board newBoard = board;

            newBoard.whitePawns.reset(to);
            newBoard.whiteKnights.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);
  
            newBoard.blackPawns.reset(from);
            if (isPromotion && to / 8 == 0)
            {
                switch (promotionPiece)
                {
                    case 'q': newBoard.blackQueen.set(to); break;
                    case 'r': newBoard.blackRooks.set(to); break;
                    case 'b': newBoard.blackBishops.set(to); break;
                    case 'n': newBoard.blackKnights.set(to); break;
                    default: return false;
                }
            }
            else
            {
                newBoard.blackPawns.set(to);
            }

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.blackRooks[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateRookMoves(from, board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;
            if (from == notationToIndex("a8")) board.blackCanCastleQueenside = false;
            else if (from == notationToIndex("h8")) board.blackCanCastleKingside = false;

            Board newBoard = board;

            newBoard.whiteKnights.reset(to);
            newBoard.whitePawns.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);

            newBoard.blackRooks.reset(from);
            newBoard.blackRooks.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.blackBishops[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateBishopMoves(from, board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;

            Board newBoard = board;

            newBoard.whiteKnights.reset(to);
            newBoard.whitePawns.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);

            newBoard.blackBishops.reset(from);
            newBoard.blackBishops.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.blackQueen[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateQueenMoves(from,board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;

            Board newBoard = board;

            newBoard.whiteKnights.reset(to);
            newBoard.whitePawns.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);

            newBoard.blackQueen.reset(from);
            newBoard.blackQueen.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;
            board.hash = Zobrist::computeHash(board);

            return true;
        }
        else if (board.blackKing[from])
        {
            board.en_passant = -1;

            if (to ==  notationToIndex("g8") && board.blackCanCastleKingside)
            {
                if (!board.getAllPieces()[notationToIndex("f8")] && !board.getAllPieces()[notationToIndex("g8")])
                {
                    Board newBoard = board;

                    newBoard.blackKing.reset(from);
                    newBoard.blackKing.set(to);
                    newBoard.blackRooks.reset(notationToIndex("h8"));
                    newBoard.blackRooks.set(notationToIndex("f8"));

                    if (isKingInCheck(newBoard) || generateWhiteAttacks(newBoard)[notationToIndex("f8")]) return false;
                    newBoard.blackCanCastleKingside = false;
                    newBoard.blackCanCastleQueenside = false;
                    newBoard.blackCastled = true;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.hash = Zobrist::computeHash(board);
                    return true;
                }
            }
            else if (to ==  notationToIndex("c8") && board.blackCanCastleQueenside)
            {
                if (!board.getAllPieces()[notationToIndex("b8")] && !board.getAllPieces()[notationToIndex("c8")] && !board.getAllPieces()[notationToIndex("d8")])
                {
                    Board newBoard = board;

                    newBoard.blackKing.reset(from);
                    newBoard.blackKing.set(to);
                    newBoard.blackRooks.reset(notationToIndex("a8"));
                    newBoard.blackRooks.set(notationToIndex("d8"));

                    if (isKingInCheck(newBoard) || generateWhiteAttacks(newBoard)[notationToIndex("d8")]) return false;
                    newBoard.blackCanCastleKingside = false;
                    newBoard.blackCanCastleQueenside = false;
                    newBoard.blackCastled = true;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.hash = Zobrist::computeHash(board);
                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, board.getBlackPieces(), board.getAllPieces()) & ~generateWhiteAttacks(board);
            if (!legalMoves[to]) return false;
            if (board.getBlackPieces()[to]) return false;

            Board newBoard = board;

            newBoard.whiteKnights.reset(to);
            newBoard.whitePawns.reset(to);
            newBoard.whiteRooks.reset(to);
            newBoard.whiteBishops.reset(to);
            newBoard.whiteQueen.reset(to);
            newBoard.whiteKing.reset(to);

            newBoard.blackKing.reset(from);
            newBoard.blackKing.set(to);

            if (isKingInCheck(newBoard)) return false;
            else board = newBoard;
            board.whiteToMove = !board.whiteToMove;

            board.blackCanCastleKingside = false;
            board.blackCanCastleQueenside = false;

            board.hash = Zobrist::computeHash(board);

            return true;
        }
    }

    return false;
}

int mirrorVertical(int sq)
{
    return sq ^ 56;
}

int getPieceValue(char piece)
{
    switch (piece)
    {
        case 'P': case 'p': return 100;
        case 'N': case 'n': return 320;
        case 'B': case 'b': return 330;
        case 'R': case 'r': return 500;
        case 'Q': case 'q': return 900;
        case 'K': case 'k': return 20000;
        default: return 0;
    }
}

bool isCheck(Board board, const std::string move)
{
    makeMove(move, board);
    return isKingInCheck(board);
}

int scoreMove(const std::string& move, const Board& board)
{
    int from = notationToIndex(move.substr(0, 2));
    int to = notationToIndex(move.substr(2, 2));
    char attacker = board.getPieceAt(from);
    char victim = board.getPieceAt(to);

    int score = 0;

    if (victim != '.')
    {
        score += 10000 + getPieceValue(victim) - getPieceValue(attacker);
    }

    if (move.length() == 5 && move[4] == 'q')
    {
        score += 8000;
    }

    if (isCheck(board, move))
    {
        score += 50;
    }

    return score;
}

std::bitset<64> getRankInFront(const Board & board, int sq)
{
    int rank = sq / 8;
    std::bitset<64> rankInFront;

    if (board.whiteToMove)
    {
        switch (rank)
        {
        case 0: rankInFront = std::bitset<64> (0x000000000000FF00); break;
        case 1: rankInFront = std::bitset<64> (0x0000000000FF0000); break;
        default: rankInFront = std::bitset<64> (0x0000000000000000);
        }
    }
    else
    {
        switch (rank)
        {
        case 7: rankInFront = std::bitset<64> (0x00FF000000000000); break;
        case 6: rankInFront = std::bitset<64> (0x0000FF0000000000); break;
        default: rankInFront = std::bitset<64> (0x0000000000000000);
        }
    }

    return rankInFront;
}

int MAX_PHASE = 24;

int gamePhase(const Board& board)
{
    int phase = 0;

    phase += board.whiteQueen.count() * 4;
    phase += board.whiteRooks.count() * 2;
    phase += board.whiteBishops.count() * 1;
    phase += board.whiteKnights.count() * 1;

    phase += board.blackQueen.count() * 4;
    phase += board.blackRooks.count() * 2;
    phase += board.blackBishops.count() * 1;
    phase += board.blackKnights.count() * 1;

    return phase;
}

std::bitset<64> getSecondRing(int kingSq, const Board& board, bool isWhiteKing)
{
    std::bitset<64> firstRing = generateKingMoves(kingSq, board.getOwnPieces(isWhiteKing), board.getAllPieces());
    std::bitset<64> secondRing;

    for (int sq = 0; sq < 64; ++sq)
    {
        if (firstRing[sq])
        {
            secondRing |= generateKingMoves(sq, board.getOwnPieces(isWhiteKing), board.getAllPieces());
        }
    }

    secondRing.reset(kingSq);
    secondRing &= ~firstRing;

    return secondRing;
}

int evaluatePosition(const Board& board) 
{
    int score = 0;
    int phase = gamePhase(board);
    double blend = static_cast<double>(phase) / MAX_PHASE;

    score += board.whitePawns.count() * 100;
    score -= board.blackPawns.count() * 100;

    score += board.whiteKnights.count() * 320;
    score -= board.blackKnights.count() * 320;

    score += board.whiteBishops.count() * 330;
    score -= board.blackBishops.count() * 330;

    score += board.whiteRooks.count() * 500;
    score -= board.blackRooks.count() * 500;

    score += board.whiteQueen.count() * 900;
    score -= board.blackQueen.count() * 900;

    score += generateWhiteAttacks(board).count() * 2;
    score -= generateBlackAttacks(board).count() * 2;

    if (board.whiteToMove && isKingInCheck(board))
    {
        score -= 30;
    }
    else if (!board.whiteToMove && isKingInCheck(board))
    {
        score += 30;
    }

    if (!board.whiteCastled && !board.whiteCanCastleKingside && !board.whiteCanCastleQueenside)
    {
        score -= 80;
    }
    if (!board.blackCastled && !board.blackCanCastleKingside && !board.blackCanCastleQueenside)
    {
        score += 80;
    }

    int castlingBonus = static_cast<int>(blend * 70);
    if (board.whiteKing._Find_first() == notationToIndex("g1"))
    {
        score += castlingBonus;
    }
    if (board.blackKing._Find_first() == notationToIndex("g8"))
    {
        score -= castlingBonus;
    }

    int kingShieldWeight = static_cast<int> (blend * 50 + (1.0 - blend) * 10);

    std::bitset<64> rankInFrontOfWhite = getRankInFront(board, board.whiteKing._Find_first());
    std::bitset<64> whiteKingMoves = generateKingMoves(board.whiteKing._Find_first(), board.getOwnPieces(true), board.getAllPieces());
    std::bitset<64> whiteShield = rankInFrontOfWhite & whiteKingMoves;
    int numShieldPawnsWhite = (board.whitePawns & whiteShield).count();
    score += numShieldPawnsWhite * kingShieldWeight;
    score -= (3 - numShieldPawnsWhite) * (kingShieldWeight / 2);

    std::bitset<64> rankInFrontOfBlack = getRankInFront(board, board.blackKing._Find_first());
    std::bitset<64> blackKingMoves = generateKingMoves(board.blackKing._Find_first(), board.getOwnPieces(false), board.getAllPieces());
    std::bitset<64> blackShield = rankInFrontOfBlack & blackKingMoves;
    int numShieldPawnsBlack = (board.blackPawns & blackShield).count();
    score -= numShieldPawnsBlack * kingShieldWeight;
    score -= (3 - numShieldPawnsBlack) * (kingShieldWeight / 2);

    const int PawnOpeningTable[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    5,  10,  10, -20, -20,  10,  10,   5,
    5,  -5, -10,   0,   0, -10,  -5,   5,
    0,   0,   0,  20,  20,   0,   0,   0,
    5,   5,  10,  25,  25,  10,   5,   5,
   10,  10,  20,  30,  30,  20,  10,  10,
   50,  50,  50,  50,  50,  50,  50,  50,
    0,   0,   0,   0,   0,   0,   0,   0
};

    const int KnightOpeningTable[64] = {
   -50, -40, -30, -30, -30, -30, -40, -50,
   -40, -20,   0,   0,   0,   0, -20, -40,
   -30,   0,  10,  15,  15,  10,   0, -30,
   -30,   5,  15,  20,  20,  15,   5, -30,
   -30,   0,  15,  20,  20,  15,   0, -30,
   -30,   5,  10,  15,  15,  10,   5, -30,
   -40, -20,   0,   5,   5,   0, -20, -40,
   -50, -40, -30, -30, -30, -30, -40, -50
};

    const int BishopOpeningTable[64] = {
   -20, -10, -10, -10, -10, -10, -10, -20,
   -10,   0,   0,   0,   0,   0,   0, -10,
   -10,   0,   5,  10,  10,   5,   0, -10,
   -10,   5,   5,  10,  10,   5,   5, -10,
   -10,   0,  10,  10,  10,  10,   0, -10,
   -10,  10,  10,  10,  10,  10,  10, -10,
   -10,   5,   0,   0,   0,   0,   5, -10,
   -20, -10, -10, -10, -10, -10, -10, -20
};

    const int RookOpeningTable[64] = {
     0,   0,   0,   5,   5,   0,   0,   0,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
     5,  10,  10,  10,  10,  10,  10,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

    const int QueenOpeningTable[64] = {
   -20, -10, -10,  -5,  -5, -10, -10, -20,
   -10,   0,   0,   0,   0,   0,   0, -10,
   -10,   0,   5,   5,   5,   5,   0, -10,
    -5,   0,   5,   5,   5,   5,   0,  -5,
     0,   0,   5,   5,   5,   5,   0,  -5,
   -10,   5,   5,   5,   5,   5,   0, -10,
   -10,   0,   5,   0,   0,   0,   0, -10,
   -20, -10, -10,  -5,  -5, -10, -10, -20
};

    const int KingOpeningTable[64] = {
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -20, -30, -30, -40, -40, -30, -30, -20,
   -10, -20, -20, -20, -20, -20, -20, -10,
    20,  20,   0,   0,   0,   0,  20,  20,
    20,  30,  10,   0,   0,  10,  30,  20
};

    const int PawnEndgameTable[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0
};

    const int KnightEndgameTable[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  15,  24,  15,  16,   0, -23,
    -29, -11,   3,   1,   5, -11, -14, -24,
    -38, -29, -27, -43, -59, -56, -39, -31
};

    const int BishopEndgameTable[64] = {
    -14, -21, -11,  -8,  -7,  -9, -17, -24,
    -8,  -4,   7, -12, -3, -13, -4, -14,
      2,  -8,   0,  -1, -2,   6,  0,   4,
     -3,   9,  12,   9, 14,  10,  3,   2,
     -6,   3,  13, 19,  7, 10,  -3,  -9,
    -12, -3,   8, 10, 13,  3, -7, -15,
    -14, -18, -7, -1,  4, -9, -15, -27,
    -23, -9, -23, -5, -9, -16, -5, -17
};

    const int RookEndgameTable[64] = {
    13, 10, 18, 15, 12, 12,   8,   5,
    11, 13, 13, 11, -3,  3,   8,   3,
     7,  7,  7,  5,  4, -3, -5, -3,
     4,  3, 13,  1,  2,   1, -1,   2,
     3,  5,  8,  4, -5, -6, -8, -11,
    -4,  0, -5, -1, -7, -12, -8, -16,
    -6, -6,  0,  2, -9, -9, -11, -3,
    -9,  2,  3, -1, -5, -13,  4, -20
};

    const int KingEndgameTable[64] = {
    -74, -35, -18, -18, -11, 15,  4, -17,
    -12, 17, 14, 17, 17, 38, 23, 11,
     10, 17, 23, 15, 20, 45, 44, 13,
     -8, 22, 24, 27, 26, 33, 26,  3,
    -18, -4, 21, 24, 27, 23,  9, -11,
    -19, -3, 11, 21, 23, 16,  7, -9,
    -27, -11,  4, 13, 14,  4, -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

    for (int sq = 0; sq < 64; ++sq)
    {
        if (board.whitePawns[sq])   score += static_cast<int> (blend * PawnOpeningTable[sq] + (1.0 - blend) * PawnEndgameTable[sq]);
        if (board.whiteKnights[sq]) score += static_cast<int> (blend * KnightOpeningTable[sq] + (1.0 - blend) * KnightEndgameTable[sq]);
        if (board.whiteBishops[sq]) score += static_cast<int> (blend * BishopOpeningTable[sq] + (1.0 - blend) * BishopEndgameTable[sq]);
        if (board.whiteRooks[sq])   score += static_cast<int> (blend * RookOpeningTable[sq] + (1.0 - blend) * RookEndgameTable[sq]);
        if (board.whiteQueen[sq])  score += QueenOpeningTable[sq];
        if (board.whiteKing[sq])    score += static_cast<int> (blend * KingOpeningTable[sq] + (1.0 - blend) * KingEndgameTable[sq]);

        if (board.blackPawns[sq])   score -= static_cast<int> (blend * PawnOpeningTable[mirrorVertical(sq)] +
                                                                       (1.0 - blend) * PawnEndgameTable[mirrorVertical(sq)]);
        if (board.blackKnights[sq]) score -= static_cast<int> (blend * KnightOpeningTable[mirrorVertical(sq)] +
                                                                       (1.0 - blend) * KnightEndgameTable[mirrorVertical(sq)]);
        if (board.blackBishops[sq]) score -= static_cast<int> (blend * BishopOpeningTable[mirrorVertical(sq)] +
                                                                       (1.0 - blend) * BishopEndgameTable[mirrorVertical(sq)]);
        if (board.blackRooks[sq])   score -= static_cast<int> (blend * RookOpeningTable[mirrorVertical(sq)] +
                                                                       (1.0 - blend) * RookEndgameTable[mirrorVertical(sq)]);
        if (board.blackQueen[sq])  score -= QueenOpeningTable[mirrorVertical(sq)];

        if (board.blackKing[sq])    score -= static_cast<int> (blend * KingOpeningTable[mirrorVertical(sq)] +
                                                                       (1.0 - blend) * KingEndgameTable[mirrorVertical(sq)]);

        std::bitset<64> piecesCloseToWhiteKing = whiteKingMoves & board.getBlackPieces();
        std::bitset<64> piecesCloseToBlackKing = blackKingMoves & board.getWhitePieces();

        if (piecesCloseToWhiteKing[sq])
        {
            if (board.blackQueen[sq])
            {
                score -= 200;
            }
            else if (board.blackRooks[sq])
            {
                score -= 130;
            }
            else if (board.blackBishops[sq])
            {
                score -= 80;
            }
            else if (board.blackKnights[sq])
            {
                score -= 80;
            }
            else if (board.blackPawns[sq])
            {
                score -= 50;
            }
        }
        if (piecesCloseToBlackKing[sq])
        {
            if (board.whiteQueen[sq])
            {
                score += 200;
            }
            else if (board.whiteRooks[sq])
            {
                score += 130;
            }
            else if (board.whiteBishops[sq])
            {
                score += 80;
            }
            else if (board.whiteKnights[sq])
            {
                score += 80;
            }
            else if (board.whitePawns[sq])
            {
                score += 50;
            }
        }

        std::bitset<64> whiteSecondRing = getSecondRing(board.whiteKing._Find_first(), board, true);
        std::bitset<64> blackSecondRing = getSecondRing(board.blackKing._Find_first(), board, false);

        if (whiteSecondRing[sq])
        {
            if (board.blackQueen[sq])
            {
                score -= 150;
            }
            else if (board.blackRooks[sq])
            {
                score -= 80;
            }
            else if (board.blackBishops[sq])
            {
                score -= 50;
            }
            else if (board.blackKnights[sq])
            {
                score -= 50;
            }
            else if (board.blackPawns[sq])
            {
                score -= 20;
            }
        }
        if (blackSecondRing[sq])
        {
            if (board.whiteQueen[sq])
            {
                score += 150;
            }
            else if (board.whiteRooks[sq])
            {
                score += 80;
            }
            else if (board.whiteBishops[sq])
            {
                score += 50;
            }
            else if (board.whiteKnights[sq])
            {
                score += 50;
            }
            else if (board.whitePawns[sq])
            {
                score += 20;
            }
        }
    }
    
    return score;
}

std::vector<std::string> generateLegalMoves(const Board& board)
{
    std::vector<std::string> legalMoves;

    std::vector<std::string> pseudoMoves;

    bool white = board.whiteToMove;

    for (int i = 0; i < 64; ++i)
    {
        std::bitset<64> from(1ULL << i);

        if ((white && board.whiteKnights[i]) || (!white && board.blackKnights[i]))
        {
            std::bitset<64> targets(generateKnightMoves(from));
            for (int j = 0; j < 64; ++j)
            {
                if (targets[j]) pseudoMoves.push_back(indexToNotation(i) + indexToNotation(j));
            }
        }

        if ((white && board.whiteBishops[i]) || (!white && board.blackBishops[i]))
        {
            std::bitset<64> targets(generateBishopMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            for (int j = 0; j < 64; ++j)
            {
                if ( targets[j]) pseudoMoves.push_back(indexToNotation(i) + indexToNotation(j));
            }
        }

        if ((white && board.whiteRooks[i]) || (!white && board.blackRooks[i]))
        {
            std::bitset<64> targets(generateRookMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            for (int j = 0; j < 64; ++j)
            {
                if ( targets[j]) pseudoMoves.push_back(indexToNotation(i) + indexToNotation(j));
            }
        }

        if ((white && board.whiteQueen[i]) || (!white && board.blackQueen[i]))
        {
            std::bitset<64> targets(generateQueenMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            for (int j = 0; j < 64; ++j)
            {
                if ( targets[j]) pseudoMoves.push_back(indexToNotation(i) + indexToNotation(j));
            }
        }

        if ((white && board.whiteKing[i]) || (!white && board.blackKing[i]))
        {
            std::bitset<64> targets(generateKingMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            std::bitset<64> enemyAttacks = white ? generateBlackAttacks(board) : generateWhiteAttacks(board);
            targets &= ~enemyAttacks;
            for (int j = 0; j < 64; ++j)
            {
                if ( targets[j]) pseudoMoves.push_back(indexToNotation(i) + indexToNotation(j));
            }
        }

        if ((white && board.whitePawns[i]) || (!white && board.blackPawns[i]))
        {
            std::bitset<64> targets = white
            ? generateWhitePawnMoves(from, board.getBlackPieces(), board.getAllPieces())
            : generateBlackPawnMoves(from, board.getWhitePieces(), board.getAllPieces());

            for (int j = 0; j < 64; ++j)
            {
                if (targets[j])
                {
                    std::string baseMove = indexToNotation(i) + indexToNotation(j);

                    if ((white && j / 8 == 7) || (!white && j / 8 == 0))
                    {
                        pseudoMoves.push_back(baseMove + 'q');
                        pseudoMoves.push_back(baseMove + 'r');
                        pseudoMoves.push_back(baseMove + 'b');
                        pseudoMoves.push_back(baseMove + 'n');
                    }
                    else pseudoMoves.push_back(baseMove);
                }
            }
        }
        if (board.whiteToMove) {

        if (board.whiteCanCastleKingside &&
            !board.getAllPieces()[notationToIndex("f1")] &&
            !board.getAllPieces()[notationToIndex("g1")] &&
            !isKingInCheck(board)) 
        {
        
            Board newBoard = board;
            newBoard.whiteKing.reset(notationToIndex("e1"));
            newBoard.whiteKing.set(notationToIndex("f1"));
            if (!generateBlackAttacks(newBoard)[notationToIndex("f1")] &&
                !generateBlackAttacks(newBoard)[notationToIndex("g1")]) 
            {
                legalMoves.push_back("e1g1");
            }
        }

        if (board.whiteCanCastleQueenside &&
            !board.getAllPieces()[notationToIndex("b1")] &&
            !board.getAllPieces()[notationToIndex("c1")] &&
            !board.getAllPieces()[notationToIndex("d1")] &&
            !isKingInCheck(board))
        {

            Board newBoard = board;
            newBoard.whiteKing.reset(notationToIndex("e1"));
            newBoard.whiteKing.set(notationToIndex("d1"));
            if (!generateBlackAttacks(newBoard)[notationToIndex("d1")] &&
                !generateBlackAttacks(newBoard)[notationToIndex("c1")]) 
            {
                legalMoves.push_back("e1c1");
            }
        }
        } else 
        {

            if (board.blackCanCastleKingside &&
                !board.getAllPieces()[notationToIndex("f8")] &&
                !board.getAllPieces()[notationToIndex("g8")] &&
                !isKingInCheck(board)) 
            {

               Board newBoard = board;
                newBoard.blackKing.reset(notationToIndex("e8"));
                newBoard.blackKing.set(notationToIndex("f8"));
                if (!generateWhiteAttacks(newBoard)[notationToIndex("f8")] &&
                    !generateWhiteAttacks(newBoard)[notationToIndex("g8")]) 
                {
                    legalMoves.push_back("e8g8");
                }
            }

            if (board.blackCanCastleQueenside &&
                !board.getAllPieces()[notationToIndex("b8")] &&
                !board.getAllPieces()[notationToIndex("c8")] &&
                !board.getAllPieces()[notationToIndex("d8")] &&
                !isKingInCheck(board)) 
            {
                Board newBoard = board;
                newBoard.blackKing.reset(notationToIndex("e8"));
                newBoard.blackKing.set(notationToIndex("d8"));
                if (!generateWhiteAttacks(newBoard)[notationToIndex("d8")] &&
                    !generateWhiteAttacks(newBoard)[notationToIndex("c8")]) 
                {
                    legalMoves.push_back("e8c8");
                }
            }
        }
    }

    for (const std::string& move : pseudoMoves)
    {
        Board newBoard = board;
        if (makeMove(move, newBoard))
        {
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}

std::vector<std::string> generateCaptures(const Board& board)
{
    std::vector<std::string> allMoves = generateLegalMoves(board);
    std::vector<std::string> captures;

    for (const std::string& move : allMoves)
    {
        int to = notationToIndex(move.substr(2, 2));
        if (board.whiteToMove)
        {
            if (board.getBlackPieces()[to])
            {
                captures.push_back(move);
            }
        }
        else
        {
            if (board.getWhitePieces()[to])
            {
                captures.push_back(move);
            }
        }
    }

    return captures;
}

std::vector<std::string> generateChecks(const Board& board)
{
    std::vector<std::string> allMoves = generateLegalMoves(board);
    std::vector<std::string> checks;

    for (const std::string& move : allMoves)
    {
        Board newBoard = board;
        makeMove(move, newBoard);

        if (isKingInCheck(newBoard))
        {
            checks.push_back(move);
        }
    }

    return checks;
}

int quiescence(Board board, int alpha, int beta, int qDepth = 0)
{
    if (qDepth > 4) return evaluatePosition(board);

    int standPat = evaluatePosition(board);

    if (board.whiteToMove)
    {
        if (standPat >= beta)
        {
            return beta;
        }
        if (alpha < standPat)
        {
            alpha = standPat;
        }
        
        std::vector<std::string>captures = generateCaptures(board);
        std::vector<std::string>checks = generateChecks(board);

        std::vector<std::string> moves = captures;
        moves.insert(moves.end(), checks.begin(), checks.end());

        for (const std::string& move : moves)
        {
            Board newBoard = board;
            makeMove(move, newBoard);

            int score = quiescence(newBoard, alpha, beta, qDepth + 1);

            if (score > alpha)
            {
                alpha = score;
                if (alpha >= beta)
                {
                    break;
                }
            }
        }
        return alpha;
    }

    else
    {
        if (standPat <= alpha)
        {
            return alpha;
        }
        if (standPat < beta)
        {
            beta = standPat;
        }

        std::vector<std::string>captures = generateCaptures(board);
        std::vector<std::string>checks = generateChecks(board);

        std::vector<std::string> moves = captures;
        moves.insert(moves.end(), checks.begin(), checks.end());

        for (const std::string& move : moves)
        {
            Board newBoard = board;
            makeMove(move, newBoard);

            int score = quiescence(newBoard, alpha, beta, qDepth + 1);

            if (score < beta)
            {
                beta = score;
                if (beta <= alpha)
                {
                    break;
                }
            }
        }
        return beta;
    }
}

int minimax(Board board, int depth, int alpha, int beta)
{
    int origAlpha = alpha;
    int origBeta = beta;    
    
    int ttScore;
    std::string ttBestMove;
    if (probeTT(board.hash, depth, alpha, beta, ttScore, ttBestMove))
    {
        return ttScore;
    }

    int score = evaluatePosition(board);
    if (depth == 0)
    {
        return quiescence(board, alpha, beta);
    }
    if (generateLegalMoves(board).empty())
    {
        if (isKingInCheck(board))
        {
            return board.whiteToMove ? -100000 : 100000;
        }
        else return 0;
    }

    std::string bestMoveFound = "";

    if (board.whiteToMove)
    {
        int best = -100000;
        for (const std::string move : generateLegalMoves(board))
        {
            Board newBoard = board;
            makeMove(move, newBoard);

            score = minimax(newBoard, depth - 1, alpha, beta);

            if (score > best)
            {
                best = score;
                bestMoveFound = move;
            }
            alpha = std::max(alpha, score);

            if (beta <= alpha)
            {
                break;
            }
        }

        TTFlag flag;
        if (best <= origAlpha) flag = UPPERBOUND;
        else if (best >= origBeta) flag = LOWERBOUND;
        else flag = EXACT;

        storeTT(board.hash, depth, best, flag, bestMoveFound);

        return best;
    }
    else
    {
        int best = 100000;
        for (const std::string move : generateLegalMoves(board))
        {
            Board newBoard = board;
            makeMove(move, newBoard);
            score = minimax(newBoard, depth - 1, alpha, beta);

            if (score < best)
            {
                best = score;
                bestMoveFound = move;
            }
            beta = std::min(beta, score);

            if (beta <= alpha)
            {
                break;
            }
        }

        TTFlag flag;
        if (best <= origAlpha) flag = UPPERBOUND;
        else if (best >= origBeta) flag = LOWERBOUND;
        else flag = EXACT;

        storeTT(board.hash, depth, best, flag, bestMoveFound);

        return best;
    }
}

std::string findBestMove(const Board& board, int maxTimeMs) {
    std::string bestMove;
    int bestScore = board.whiteToMove ? -100000 : 100000;
    auto start = std::chrono::steady_clock::now();

    for (int depth = 1; ; ++depth) {
        std::string currentBestMove;
        int currentBestScore = board.whiteToMove ? -100000 : 100000;

        std::vector<std::string> moves = generateLegalMoves(board);

        std::sort(moves.begin(), moves.end(), [&](const std::string& a, const std::string& b) {
            return scoreMove(a, board) > scoreMove(b, board);
        });

        for (const std::string& move : moves) {
            Board newBoard = board;
            makeMove(move, newBoard);
            int score = minimax(newBoard, depth - 1, -100000, 100000);

            if (board.whiteToMove && score > currentBestScore) {
                currentBestScore = score;
                currentBestMove = move;
            } else if (!board.whiteToMove && score < currentBestScore) {
                currentBestScore = score;
                currentBestMove = move;
            }

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > maxTimeMs) {
                std::cerr << "Time up at depth " << depth << "\n";
                return bestMove.empty() ? currentBestMove : bestMove;
            }
        }

        bestMove = currentBestMove;
        bestScore = currentBestScore;

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > maxTimeMs) {
            break;
        }
    }

    return bestMove;
}

int main()
{
    Zobrist::initializeKeys();
    uciLoop();
    return 0;
}