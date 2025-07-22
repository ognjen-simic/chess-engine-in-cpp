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

char Board::getPieceAt(int index) const {
    for (int i = 0; i < 64; ++i) {
        if (whitePawns[i] && i == index) return 'P';
        if (blackPawns[i] && i == index) return 'p';
        if (whiteKnights[i] && i == index) return 'N';
        if (blackKnights[i] && i == index) return 'n';
        if (whiteBishops[i] && i == index) return 'B';
        if (blackBishops[i] && i == index) return 'b';
        if (whiteRooks[i] && i == index) return 'R';
        if (blackRooks[i] && i == index) return 'r';
        if (whiteQueen[i] && i == index) return 'Q';
        if (blackQueen[i] && i == index) return 'q';
        if (whiteKing[i] && i == index) return 'K';
        if (blackKing[i] && i == index) return 'k';
    }
    return '.';
}
