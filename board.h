#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include <string>

struct Board
{
    std::bitset<64> whitePawns;
    std::bitset<64> whiteKnights;
    std::bitset<64> whiteBishops;
    std::bitset<64> whiteRooks;
    std::bitset<64> whiteQueen;
    std::bitset<64> whiteKing;

    std::bitset<64> blackPawns;
    std::bitset<64> blackKnights;
    std::bitset<64> blackBishops;
    std::bitset<64> blackRooks;
    std::bitset<64> blackQueen;
    std::bitset<64> blackKing;
    
    bool whiteCanCastleKingside = true;
    bool whiteCanCastleQueenside = true;
    bool blackCanCastleKingside = true;
    bool blackCanCastleQueenside = true;
    bool whiteCastled = false;
    bool blackCastled = false;
    bool whiteToMove = true;
    int en_passant = -1;

std::bitset<64> getWhitePieces() const;
std::bitset<64> getBlackPieces() const;
std::bitset<64> getAllPieces() const;
std::bitset<64> getOwnPieces(bool whiteToMove) const;
std::bitset<64> getOpponentPieces(bool whiteToMove) const;
char getPieceAt(int index) const;
};

#endif