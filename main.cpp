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
#include "move.h"

long long nodes = 0;

enum Square : int {
    SQ_A1 = 0, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE = 64
};

std::bitset<64> generateKnightMoves(std::bitset<64> knights)
 {
    std::bitset<64> notAFile = ~std::bitset<64>(0x0101010101010101ULL);
    std::bitset<64> notABFile = ~std::bitset<64>(0x0303030303030303ULL);
    std::bitset<64> notHFile = ~std::bitset<64>(0x8080808080808080ULL);
    std::bitset<64> notGHFile = ~std::bitset<64>(0xC0C0C0C0C0C0C0C0ULL);

    std::bitset<64> result;

    result |= (knights & notAFile) << 15; 
    result |= (knights & notHFile) << 17;
    result |= (knights & notABFile) << 6;
    result |= (knights & notGHFile) << 10;

    result |= (knights & notAFile) >> 17;
    result |= (knights & notHFile) >> 15;
    result |= (knights & notABFile) >> 10;
    result |= (knights & notGHFile) >> 6;

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
    bool white = board.whiteToMove;

    int kingSquare = (white ? board.whiteKing : board.blackKing)._Find_first();

    if (kingSquare >= 64) {
        std::cerr << "Error: King not found on board!" << std::endl;
        return std::map<int, std::bitset<64>>();
    }

    std::bitset<64> enemyBishops = white ? board.blackBishops : board.whiteBishops;
    std::bitset<64> enemyRooks = white ? board.blackRooks : board.whiteRooks;
    std::bitset<64> enemyQueens = white ? board.blackQueen : board.whiteQueen;

    std::bitset<64> ownPieces = board.getOwnPieces(white);
    std::bitset<64> allPieces    = board.getAllPieces();

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

void removePieceAt(Board& b, int sq) 
{
    if (!b.getAllPieces()[sq]) return;

    b.whitePawns.reset(sq);
    b.whiteKnights.reset(sq);
    b.whiteBishops.reset(sq);
    b.whiteRooks.reset(sq);
    b.whiteQueen.reset(sq);
    b.whiteKing.reset(sq);

    b.blackPawns.reset(sq);
    b.blackKnights.reset(sq);
    b.blackBishops.reset(sq);
    b.blackRooks.reset(sq);
    b.blackQueen.reset(sq);
    b.blackKing.reset(sq);
}

bool makeMove(Move move, Board& board)
{
    int from = MoveEncoder::getFrom(move);
    int to = MoveEncoder::getTo(move);
    int flag = MoveEncoder::getFlag(move);

    if (from == -1 || to == -1) return false;

    bool isPromotion = (flag >= MoveEncoder::PROMO_KNIGHT) && (flag <= MoveEncoder::PROMO_QUEEN);

    if (board.getAllPieces()[to]) {
        removePieceAt(board, to);
    }

    if (board.whiteToMove) {
        if (board.whitePawns[from]) {
            board.whitePawns.reset(from);
         
            if (to == board.en_passant && (to / 8 == 5)) {
                board.blackPawns.reset(to - 8);
            }

            if (isPromotion) {
                switch (flag) {
                    case MoveEncoder::PROMO_QUEEN: board.whiteQueen.set(to); break;
                    case MoveEncoder::PROMO_ROOK: board.whiteRooks.set(to); break;
                    case MoveEncoder::PROMO_BISHOP: board.whiteBishops.set(to); break;
                    case MoveEncoder::PROMO_KNIGHT: board.whiteKnights.set(to); break;
                }
            } else {
                board.whitePawns.set(to);
            }
            
            if (from / 8 == 1 && to / 8 == 3) board.en_passant = from + 8;
            else board.en_passant = -1;
        }
        else if (board.whiteKnights[from]) {
            board.whiteKnights.reset(from);
            board.whiteKnights.set(to);
            board.en_passant = -1;
        }
        else if (board.whiteBishops[from]) {
            board.whiteBishops.reset(from);
            board.whiteBishops.set(to);
            board.en_passant = -1;
        }
        else if (board.whiteRooks[from]) {
            board.whiteRooks.reset(from);
            board.whiteRooks.set(to);
            board.en_passant = -1;

            if (from == SQ_A1) board.whiteCanCastleQueenside = false;
            else if (from == SQ_H1) board.whiteCanCastleKingside = false;
        }
        else if (board.whiteQueen[from]) {
            board.whiteQueen.reset(from);
            board.whiteQueen.set(to);
            board.en_passant = -1;
        }
        else if (board.whiteKing[from]) {
            board.whiteKing.reset(from);
            board.whiteKing.set(to);
            board.en_passant = -1;
   
            if (from == SQ_E1) {
                if (to == SQ_G1) {
                    board.whiteRooks.reset(SQ_H1);
                    board.whiteRooks.set(SQ_F1);
                    board.whiteCastled = true;
                } else if (to == SQ_C1) { 
                    board.whiteRooks.reset(SQ_A1);
                    board.whiteRooks.set(SQ_D1);
                    board.whiteCastled = true;
                }
            }
            board.whiteCanCastleKingside = false;
            board.whiteCanCastleQueenside = false;
        }
    } 
    else { 
        if (board.blackPawns[from]) {
            board.blackPawns.reset(from);

            if (to == board.en_passant && (to / 8 == 2)) {
                board.whitePawns.reset(to + 8);
            }

            if (isPromotion) {
                switch (flag) {
                    case MoveEncoder::PROMO_QUEEN: board.blackQueen.set(to); break;
                    case MoveEncoder::PROMO_ROOK: board.blackRooks.set(to); break;
                    case MoveEncoder::PROMO_BISHOP: board.blackBishops.set(to); break;
                    case MoveEncoder::PROMO_KNIGHT: board.blackKnights.set(to); break;
                }
            } else {
                board.blackPawns.set(to);
            }

            if (from / 8 == 6 && to / 8 == 4) board.en_passant = from - 8;
            else board.en_passant = -1;
        }
        else if (board.blackKnights[from]) {
            board.blackKnights.reset(from);
            board.blackKnights.set(to);
            board.en_passant = -1;
        }
        else if (board.blackBishops[from]) {
            board.blackBishops.reset(from);
            board.blackBishops.set(to);
            board.en_passant = -1;
        }
        else if (board.blackRooks[from]) {
            board.blackRooks.reset(from);
            board.blackRooks.set(to);
            board.en_passant = -1;
            
            if (from == SQ_A8) board.blackCanCastleQueenside = false;
            else if (from == SQ_H8) board.blackCanCastleKingside = false;
        }
        else if (board.blackQueen[from]) {
            board.blackQueen.reset(from);
            board.blackQueen.set(to);
            board.en_passant = -1;
        }
        else if (board.blackKing[from]) {
            board.blackKing.reset(from);
            board.blackKing.set(to);
            board.en_passant = -1;

            if (from == SQ_E8) {
                if (to == SQ_G8) { 
                    board.blackRooks.reset(SQ_H8);
                    board.blackRooks.set(SQ_F8);
                    board.blackCastled = true;
                } else if (to == SQ_C8) { 
                    board.blackRooks.reset(SQ_A8);
                    board.blackRooks.set(SQ_D8);
                    board.blackCastled = true;
                }
            }
            board.blackCanCastleKingside = false;
            board.blackCanCastleQueenside = false;
        }
    }

    board.whiteToMove = !board.whiteToMove;
    board.hash = Zobrist::computeHash(board);

    return true;
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

bool isCheck(Board board, const Move move)
{
    makeMove(move, board);
    return isKingInCheck(board);
}

constexpr int MAX_DEPTH = 64;
Move killerMoves[MAX_DEPTH][2];

int scoreMove(const Move& move, const Board& board, int depth)
{
    int from = MoveEncoder::getFrom(move);
    int to = MoveEncoder::getTo(move);
    char attacker = board.getPieceAt(from);
    char victim = board.getPieceAt(to);

    int score = 0;

    if (victim != '.')
    {
        score += 10000 + getPieceValue(victim) - getPieceValue(attacker);
    }

    if (MoveEncoder::getFlag(move) == MoveEncoder::PROMO_QUEEN)
    {
        score += 8000;
    }
    if (victim == '.')
    {
        if (killerMoves[depth][0] == move) score += 9000;
        else if (killerMoves[depth][1] == move) score += 8000;
    }

    return score;
}

std::bitset<64> getRankInFront(const Board & board, int sq)
{
    int rank = sq / 8;
    std::bitset<64> rankInFront;

    if (rank < 4)
    {
        if (rank < 4) {
        if (rank == 0) return std::bitset<64>(0x000000000000FF00ULL); 
        if (rank == 1) return std::bitset<64>(0x0000000000FF0000ULL); 
        return std::bitset<64>(0);
        } 
    else {
        if (rank == 7) return std::bitset<64>(0x00FF000000000000ULL);
        if (rank == 6) return std::bitset<64>(0x0000FF0000000000ULL);
        return std::bitset<64>(0);
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
    if (kingSq >= 64) return std::bitset<64>(0);

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

bool isPassedPawn(int sq, bool white,  const Board& board)
{
    std::bitset<64> enemyPawns = white ? board.blackPawns : board.whitePawns;
    std::bitset<64> mask = 0ULL;

    int file = sq % 8;
    int rank = sq / 8;

    for (int df = -1; df <= 1; ++df)
    {
        int f = file + df;
        if (f < 0 || f > 7) continue;

        if (white)
        {
            for (int r = rank + 1; r < 8; ++r)
            {
            int targetSq = r * 8 + f;
            mask |= (1ULL << targetSq);
            }
        }
        else
        {
            for (int r = rank - 1; r > 0; --r)
            {
            int targetSq = r * 8 + f;
            mask |= (1ULL << targetSq);
            }
        }
    }

    return (enemyPawns & mask) == 0;
}

int pawnProtections(int sq, bool white, const Board& board)
{
    std::bitset<64> protSq = white ? constBlackPawnCaptures(sq) : constWhitePawnCaptures(sq);
    std::bitset<64> ownPawns = white ? board.whitePawns : board.blackPawns;

    return (protSq & ownPawns).count();
}

std::bitset<64> fileMask(int file)
{
    std::bitset<64> mask;

    for (int rank = 1; rank < 7; ++rank)
    {
        mask.set(rank * 8 + file);
     }

    return mask;
}

bool isIsolatedPawn(int sq, bool white, const Board& board)
{
    int file = sq % 8;
    std::bitset<64> ownPawns = white ? board.whitePawns : board.blackPawns;

    bool leftHasPawn = false;
    bool rightHasPawn = false;

    if (file > 0)
    {
        leftHasPawn = (ownPawns & fileMask(file - 1)).any();
    }
    if (file < 7)
    {
        rightHasPawn = (ownPawns & fileMask(file + 1)).any();
    }

    return !(leftHasPawn || rightHasPawn);
}

int doubledPawnPenalty(bool white, const Board& board)
{
    std::bitset<64> pawns = white ? board.whitePawns : board.blackPawns;
    int penalty = 0;

    for (int file = 0; file < 8; ++file)
    {
        int count = 0;
        for (int rank = 1; rank < 7; ++rank)
        {
            int sq = rank * 8 + file;
            if (pawns[sq]) count++;
        }

        if (count > 1)
        {
            penalty += (count - 1) * 5;
        }
    }
    return penalty;
}

int distance(int sq1, int sq2)
{
    int file1 = sq1 % 8;
    int rank1 = sq1 / 8;
    int file2 = sq2 % 8;
    int rank2 = sq2 / 8;

    return std::max(std::abs(file1 - file2), std::abs(rank1 - rank2));
}

int centerManhattanDistance(int sq)
{
    int file = sq % 8;
    int rank = sq / 8;

    int distFile = std::abs(2*file-7);
    int distRank = std::abs(2*rank-7);
    return distFile + distRank;
}

int evaluatePosition(const Board& board)
{
    int whiteKingSq = board.whiteKing._Find_first();
    int blackKingSq = board.blackKing._Find_first();

    if (whiteKingSq >= 64) return -100000;
    if (blackKingSq >= 64) return  100000;

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
        20,  32,  12,   0,   0,  10,  35,  20
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

    auto evalPieceSet = [&](std::bitset<64> pieces, const int* opTable, const int* endTable, bool isWhite) 
    {
        int setScore = 0;
        while (pieces.any()) 
        {
            int sq = pieces._Find_first();
            pieces.reset(sq);
            int tableSq = isWhite ? sq : mirrorVertical(sq);
            setScore += static_cast<int>(blend * opTable[tableSq] + (1.0 - blend) * endTable[tableSq]);
        }
        return isWhite ? setScore : -setScore;
    };

    score += evalPieceSet(board.whiteKnights, KnightOpeningTable, KnightEndgameTable, true);
    score += evalPieceSet(board.blackKnights, KnightOpeningTable, KnightEndgameTable, false);
    score += evalPieceSet(board.whiteBishops, BishopOpeningTable, BishopEndgameTable, true);
    score += evalPieceSet(board.blackBishops, BishopOpeningTable, BishopEndgameTable, false);
    score += evalPieceSet(board.whiteRooks,   RookOpeningTable,   RookEndgameTable,   true);
    score += evalPieceSet(board.blackRooks,   RookOpeningTable,   RookEndgameTable,   false);
    score += evalPieceSet(board.whiteQueen,   QueenOpeningTable,  QueenOpeningTable,  true);
    score += evalPieceSet(board.blackQueen,   QueenOpeningTable,  QueenOpeningTable,  false);
    score += evalPieceSet(board.whiteKing,    KingOpeningTable,   KingEndgameTable,   true);
    score += evalPieceSet(board.blackKing,    KingOpeningTable,   KingEndgameTable,   false);

    std::bitset<64> wp = board.whitePawns;
    while (wp.any()) 
    {
        int sq = wp._Find_first();
        wp.reset(sq);
        score += static_cast<int>(blend * PawnOpeningTable[sq] + (1.0 - blend) * PawnEndgameTable[sq]);

        if (isPassedPawn(sq, true, board))
        {
            int rank = sq / 8;
            score += rank * 10;
        }
        if (isIsolatedPawn(sq, true, board)) score -= 8 * (1.0 - blend);
        score += 10 * pawnProtections(sq, true, board) * (1.0 - blend);
    }

    std::bitset<64> bp = board.blackPawns;
    while (bp.any()) 
    {
        int sq = bp._Find_first();
        bp.reset(sq);
        int mSq = mirrorVertical(sq);
        score -= static_cast<int>(blend * PawnOpeningTable[mSq] + (1.0 - blend) * PawnEndgameTable[mSq]);

        if (isPassedPawn(sq, false, board))
        {
            int rank = sq / 8;
            int accRank = 7 - rank;
            score -= accRank * 10;
        }
        if (isIsolatedPawn(sq, false, board)) score += 8 * (1.0 - blend);
        score -= 10 * pawnProtections(sq, false, board) * (1.0 - blend);
    }

    score -= doubledPawnPenalty(true, board) * (1.0 - blend);
    score += doubledPawnPenalty(false, board) * (1.0 - blend);

    std::bitset<64> whiteAttacks = generateWhiteAttacks(board);
    std::bitset<64> blackAttacks = generateBlackAttacks(board);

    score += whiteAttacks.count();
    score -= blackAttacks.count();

    if (board.whiteToMove){
        if (blackAttacks[whiteKingSq]) score -= 30;
    }
    else{
        if (whiteAttacks[blackKingSq]) score += 30;
    }

    if (!board.whiteCastled && !board.whiteCanCastleKingside && !board.whiteCanCastleQueenside) score -= 80;
    if (!board.blackCastled && !board.blackCanCastleKingside && !board.blackCanCastleQueenside) score += 80;

    int kingShieldWeight = static_cast<int>(blend * 50 + (1.0 - blend) * 10);
    
    std::bitset<64> rankInFrontOfWhite = getRankInFront(board, whiteKingSq);
    std::bitset<64> whiteKingMoves = generateKingMoves(whiteKingSq, board.getOwnPieces(true), board.getAllPieces());
    std::bitset<64> whiteShield = rankInFrontOfWhite & whiteKingMoves;
    int numShieldPawnsWhite = (board.whitePawns & whiteShield).count();
    score += numShieldPawnsWhite * kingShieldWeight;
    score -= (3 - numShieldPawnsWhite) * (kingShieldWeight / 2);

    std::bitset<64> rankInFrontOfBlack = getRankInFront(board, blackKingSq);
    std::bitset<64> blackKingMoves = generateKingMoves(blackKingSq, board.getOwnPieces(false), board.getAllPieces());
    std::bitset<64> blackShield = rankInFrontOfBlack & blackKingMoves;
    int numShieldPawnsBlack = (board.blackPawns & blackShield).count();
    score -= numShieldPawnsBlack * kingShieldWeight;
    score += (3 - numShieldPawnsBlack) * (kingShieldWeight / 2);

    std::bitset<64> piecesCloseToWhiteKing = whiteKingMoves & board.getBlackPieces();
    while (piecesCloseToWhiteKing.any())
    {
        int sq = piecesCloseToWhiteKing._Find_first();
        piecesCloseToWhiteKing.reset(sq);
        if (board.blackQueen[sq]) score -= 60;
        else if (board.blackRooks[sq]) score -= 40;
        else if (board.blackBishops[sq]) score -= 20;
        else if (board.blackKnights[sq]) score -= 20;
        else if (board.blackPawns[sq]) score -= 10;
    }

    std::bitset<64> piecesCloseToBlackKing = blackKingMoves & board.getWhitePieces();
    while (piecesCloseToBlackKing.any())
    {
        int sq = piecesCloseToBlackKing._Find_first();
        piecesCloseToBlackKing.reset(sq);
        if (board.whiteQueen[sq]) score += 60;
        else if (board.whiteRooks[sq]) score += 40;
        else if (board.whiteBishops[sq]) score += 20;
        else if (board.whiteKnights[sq]) score += 20;
        else if (board.whitePawns[sq]) score += 10;
    }

    std::bitset<64> whiteSecondRing = getSecondRing(whiteKingSq, board, true);
    while (whiteSecondRing.any())
    {
        int sq = whiteSecondRing._Find_first();
        whiteSecondRing.reset(sq);
        if (board.blackQueen[sq]) score -= 45;
        else if (board.blackRooks[sq]) score -= 15;
        else if (board.blackBishops[sq]) score -= 10;
        else if (board.blackKnights[sq]) score -= 10;
        else if (board.blackPawns[sq]) score -= 5;
    }

    std::bitset<64> blackSecondRing = getSecondRing(blackKingSq, board, false);
    while (blackSecondRing.any())
    {
        int sq = blackSecondRing._Find_first();
        blackSecondRing.reset(sq);
        if (board.whiteQueen[sq]) score += 45;
        else if (board.whiteRooks[sq]) score += 15;
        else if (board.whiteBishops[sq]) score += 10;
        else if (board.whiteKnights[sq]) score += 10;
        else if (board.whitePawns[sq]) score += 5;
    }

    int mopUpScore = 0;

    if (score > 0)
    {
        int cmd = centerManhattanDistance(blackKingSq);
        int md = distance(whiteKingSq, blackKingSq);
        mopUpScore += (cmd * 4) + 10 * (14 - md);

        score += static_cast<int>(mopUpScore * (1 - blend));
    }
    else if (score < 0)
    {
        int cmd = centerManhattanDistance(whiteKingSq);
        int md = distance(blackKingSq, whiteKingSq);
        mopUpScore += (cmd * 4) + 10 * (14 - md);

        score -= static_cast<int>(mopUpScore * (1 - blend));
    }

    return score;
}

std::string moveToUCI(Move m) 
{
    if (m == 0) return "0000";
    
    int from = MoveEncoder::getFrom(m);
    int to = MoveEncoder::getTo(m);
    int flag = MoveEncoder::getFlag(m);

    std::string s = indexToNotation(from) + indexToNotation(to);

    if (flag == MoveEncoder::PROMO_QUEEN) s += 'q';
    else if (flag == MoveEncoder::PROMO_ROOK) s += 'r';
    else if (flag == MoveEncoder::PROMO_BISHOP) s += 'b';
    else if (flag == MoveEncoder::PROMO_KNIGHT) s += 'n';

    return s;
}

std::vector<Move> generateLegalMoves(const Board& board)
{
    std::vector<Move> legalMoves;
    std::vector<Move> pseudoMoves;
    bool white = board.whiteToMove;

    std::map<int, std::bitset<64>> pinnedLines = getPinnedPieces(board);

    for (int i = 0; i < 64; ++i) {
        std::bitset<64> from(1ULL << i);

        if ((white && board.whiteKnights[i]) || (!white && board.blackKnights[i])) {
            std::bitset<64> targets(generateKnightMoves(from));
            targets &= ~board.getOwnPieces(white);
            for (int j = 0; j < 64; ++j) if (targets[j]) pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
        }

        if ((white && board.whiteBishops[i]) || (!white && board.blackBishops[i])) {
            std::bitset<64> targets(generateBishopMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            targets &= ~board.getOwnPieces(white);
            for (int j = 0; j < 64; ++j) if (targets[j]) pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
        }

        if ((white && board.whiteRooks[i]) || (!white && board.blackRooks[i])) {
            std::bitset<64> targets(generateRookMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            targets &= ~board.getOwnPieces(white);
            for (int j = 0; j < 64; ++j) if (targets[j]) pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
        }

        if ((white && board.whiteQueen[i]) || (!white && board.blackQueen[i])) {
            std::bitset<64> targets(generateQueenMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            targets &= ~board.getOwnPieces(white);
            for (int j = 0; j < 64; ++j) if (targets[j]) pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
        }

        if ((white && board.whiteKing[i]) || (!white && board.blackKing[i])) {
            std::bitset<64> targets(generateKingMoves(i, board.getOwnPieces(white), board.getAllPieces()));
            std::bitset<64> enemyAttacks = white ? generateBlackAttacks(board) : generateWhiteAttacks(board);
            targets &= ~enemyAttacks;
            for (int j = 0; j < 64; ++j) if (targets[j]) pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
        }

        if ((white && board.whitePawns[i]) || (!white && board.blackPawns[i])) {
            std::bitset<64> targets = white
                ? generateWhitePawnMoves(from, board.getBlackPieces(), board.getAllPieces())
                : generateBlackPawnMoves(from, board.getWhitePieces(), board.getAllPieces());

            for (int j = 0; j < 64; ++j) {
                if (!targets[j]) continue;
                if ((white && j / 8 == 7) || (!white && j / 8 == 0)) {
                    pseudoMoves.push_back(MoveEncoder::encode(i, j, MoveEncoder::PROMO_QUEEN));
                    pseudoMoves.push_back(MoveEncoder::encode(i, j, MoveEncoder::PROMO_ROOK));
                    pseudoMoves.push_back(MoveEncoder::encode(i, j, MoveEncoder::PROMO_BISHOP));
                    pseudoMoves.push_back(MoveEncoder::encode(i, j, MoveEncoder::PROMO_KNIGHT));
                } else {
                    pseudoMoves.push_back(MoveEncoder::encode(i, j, 0));
                }
            }

            int epSquare = board.en_passant;
            if (epSquare != -1) {
                if (white && (i / 8) == 4) {
                    if (abs((epSquare % 8) - (i % 8)) == 1) {
                        pseudoMoves.push_back(MoveEncoder::encode(i, epSquare, 0));
                    }
                } else if (!white && (i / 8) == 3) {
                    if (abs((epSquare % 8) - (i % 8)) == 1) {
                        pseudoMoves.push_back(MoveEncoder::encode(i, epSquare, 0));
                    }
                }
            }
        }
    }

    std::bitset<64> all = board.getAllPieces();

    if (board.whiteToMove) {
         if (board.whiteCanCastleKingside && !all[SQ_F1] && !all[SQ_G1] && !isKingInCheck(board))
        {
            Board newBoard = board;
            newBoard.whiteKing.reset(SQ_E1);
            newBoard.whiteKing.set(SQ_F1);
            if (!generateBlackAttacks(newBoard)[SQ_F1] && !generateBlackAttacks(newBoard)[SQ_G1]) {
                legalMoves.push_back(MoveEncoder::encode(SQ_E1, SQ_G1, MoveEncoder::QUIET));
            }
        }

         if (board.whiteCanCastleQueenside && !all[SQ_B1] && !all[SQ_C1] && !all[SQ_D1] && !isKingInCheck(board))
        {
            Board newBoard = board;
            newBoard.whiteKing.reset(SQ_E1);
            newBoard.whiteKing.set(SQ_D1);
            if (!generateBlackAttacks(newBoard)[SQ_D1] && !generateBlackAttacks(newBoard)[SQ_C1]) {
                legalMoves.push_back(MoveEncoder::encode(SQ_E1, SQ_C1, MoveEncoder::QUIET));
            }
        }
    } else {
        if (board.blackCanCastleKingside && !all[SQ_F8] && !all[SQ_G8] && !isKingInCheck(board))
        {
            Board newBoard = board;
            newBoard.blackKing.reset(SQ_E8);
            newBoard.blackKing.set(SQ_F8);
            if (!generateWhiteAttacks(newBoard)[SQ_F8] && !generateWhiteAttacks(newBoard)[SQ_G8]) {
                legalMoves.push_back(MoveEncoder::encode(SQ_E8, SQ_G8, MoveEncoder::QUIET));
            }
        }
         if (board.blackCanCastleQueenside && !all[SQ_B8] && !all[SQ_C8] && !all[SQ_D8] && !isKingInCheck(board))
        {
            Board newBoard = board;
            newBoard.blackKing.reset(SQ_E8);
            newBoard.blackKing.set(SQ_D8);
            if (!generateWhiteAttacks(newBoard)[SQ_D8] && !generateWhiteAttacks(newBoard)[SQ_C8]) {
                legalMoves.push_back(MoveEncoder::encode(SQ_E8, SQ_C8, MoveEncoder::QUIET));
            }
        }
    }

    for (const Move &m : pseudoMoves) {
        int from = MoveEncoder::getFrom(m);
        int to   = MoveEncoder::getTo(m);

        if (board.whiteToMove) {
            if (board.blackKing[to]) continue;
        } else {
            if (board.whiteKing[to]) continue;
        }

        if (pinnedLines.count(from)) {
            if (!pinnedLines[from][to]) continue;
        }

        Board newBoard = board;
        makeMove(m, newBoard);

        int kingSq;
        if (board.whiteToMove) {
             kingSq = newBoard.whiteKing._Find_first();
             if (generateBlackAttacks(newBoard)[kingSq]) continue;
        } else { 
             kingSq = newBoard.blackKing._Find_first();
             if (generateWhiteAttacks(newBoard)[kingSq]) continue;
        }

        legalMoves.push_back(m);
    }

    return legalMoves;
}

std::vector<Move> generateCaptures(const Board& board)
{
    std::vector<Move> allMoves = generateLegalMoves(board);
    std::vector<Move> captures;

    for (const Move& move : allMoves)
    {
        int to = MoveEncoder::getTo(move);
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

        if (to == board.en_passant && board.en_passant != -1)
        {
            int from = MoveEncoder::getFrom(move);
            if (board.whiteToMove && board.whitePawns[from]) captures.push_back(move);
            else if (!board.whiteToMove && board.blackPawns[from]) captures.push_back(move);
        }
    }

    return captures;
}

std::vector<Move> generateChecks(const Board& board)
{
    std::vector<Move> allMoves = generateLegalMoves(board);
    std::vector<Move> checks;

    for (const Move& move : allMoves)
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

Move parseMove(const std::string& moveStr, const Board& board)
{
    std::vector<Move> moves = generateLegalMoves(board);
    
    for (Move m : moves) {
        if (moveToUCI(m) == moveStr) return m;
    }
    return 0;
}

int quiescence(Board board, int alpha, int beta, int qDepth = 0)
{
    nodes++;

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
        
        std::vector<Move>captures = generateCaptures(board);

        for (const Move& move : captures)
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

        std::vector<Move>captures = generateCaptures(board);

        for (const Move& move : captures)
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
    nodes++;

    int origAlpha = alpha;
    int origBeta = beta;    
    
    int ttScore;
    Move ttBestMove;
    if (probeTT(board.hash, depth, alpha, beta, ttScore, ttBestMove))
    {
        return ttScore;
    }

    int score = evaluatePosition(board);
    if (depth == 0)
    {
        return quiescence(board, alpha, beta);
    }

    std::vector<Move> moves = generateLegalMoves(board);

    if (moves.empty())
    {
        if (isKingInCheck(board))
        {
            return board.whiteToMove ? -100000 : 100000;
        }
        else return 0;
    }

    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) 
    {
        return scoreMove(a, board, depth) > scoreMove(b, board, depth);
    });

    Move bestMoveFound = 0;

    if (board.whiteToMove)
    {
        int best = -100000;
        for (const Move move : moves)
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
                if (board.getPieceAt(MoveEncoder::getTo(move)) == '.') 
                {
                    if (killerMoves[depth][0] != move)
                    {
                        killerMoves[depth][1] = killerMoves[depth][0];
                        killerMoves[depth][0] = move;
                    }
                }
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
        for (const Move move : moves)
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
                if (board.getPieceAt(MoveEncoder::getTo(move)) == '.') 
                {
                    if (killerMoves[depth][0] != move)
                    {
                        killerMoves[depth][1] = killerMoves[depth][0];
                        killerMoves[depth][0] = move;
                    }
                }
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

Move findBestMove(const Board& board, int maxTimeMs)
{   
    nodes = 0;

    Move bestMove = 0;
    int bestScore = board.whiteToMove ? -100000 : 100000;
    auto start = std::chrono::steady_clock::now();

    for (int depth = 1; ; ++depth) {
        Move currentBestMove = 0;
        int currentBestScore = board.whiteToMove ? -100000 : 100000;

        std::vector<Move> moves = generateLegalMoves(board);

        Move ttMove;
        int ttScore;
        if (probeTT(board.hash, depth, -100000, 100000, ttScore, ttMove)) {
            auto it = std::find(moves.begin(), moves.end(), ttMove);
            if (it != moves.end()) {
                std::iter_swap(moves.begin(), it);
            }
        }

        std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
            return scoreMove(a, board, depth) > scoreMove(b, board, depth);
        });

        for (const Move& move : moves) {
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
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > maxTimeMs) 
            {
                auto end = std::chrono::steady_clock::now();
                long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                if (duration == 0) duration = 1;
                long long nps = (nodes * 1000) / duration;

                std::cout << "info depth " << depth 
                          << " nodes " << nodes 
                          << " time " << duration 
                          << " nps " << nps 
                          << " score cp " << bestScore
                          << " pv " << moveToUCI(bestMove == 0 ? currentBestMove : bestMove) 
                          << "\n";

                return bestMove == 0 ? currentBestMove : bestMove;
            }
        }

        bestMove = currentBestMove;
        bestScore = currentBestScore;

    }
    return bestMove;
}

int main()
{
    Zobrist::initializeKeys();
    uciLoop();
    return 0;
}