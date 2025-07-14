#include "Board.h"

std::bitset<64> Board::getWhitePieces() const
{
    return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
}

std::bitset<64> Board::getBlackPieces() const
{
    return blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing;
}

std::bitset<64> Board::getAllPieces() const
{
    return getWhitePieces() | getBlackPieces();
}

std::bitset<64> Board::getOwnPieces(bool whiteToMove) const
{
    return whiteToMove 
    ? whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing
    : blackPawns | blackKnights | blackBishops |blackRooks | blackQueen | blackKing;
}

std::bitset<64> Board::getOpponentPieces(bool whiteToMove) const {
return whiteToMove
    ? blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing
    : whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
}
