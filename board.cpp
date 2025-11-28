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
    ? getWhitePieces()
    : getBlackPieces();
}

std::bitset<64> Board::getOpponentPieces(bool whiteToMove) const {
return whiteToMove
    ? getBlackPieces()
    : getWhitePieces();
}

char Board::getPieceAt(int index) const {

    if (index < 0 || index >= 64) return '.';

    if (whitePawns[index])   return 'P';
    if (whiteKnights[index]) return 'N';
    if (whiteBishops[index]) return 'B';
    if (whiteRooks[index])   return 'R';
    if (whiteQueen[index])   return 'Q';
    if (whiteKing[index])    return 'K';

    if (blackPawns[index])   return 'p';
    if (blackKnights[index]) return 'n';
    if (blackBishops[index]) return 'b';
    if (blackRooks[index])   return 'r';
    if (blackQueen[index])   return 'q';
    if (blackKing[index])    return 'k';

    return '.';
}

bool Board::isEnemyPiece(char piece, bool white) const {
    if (piece == '.') return false;

    if (white) {
        return (piece >= 'a' && piece <= 'z');
    }
    
    else {
        return (piece >= 'A' && piece <= 'Z');
    }
}