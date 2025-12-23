#include "Board.h"
#include <sstream>
#include <cctype>
#include "zobrist.h"

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

void Board::setFromFEN(const std::string& fen) {
    whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueen = whiteKing = 0;
    blackPawns = blackKnights = blackBishops = blackRooks = blackQueen = blackKing = 0;
    
    std::istringstream iss(fen);
    std::string boardPart, activeColor, castling, enPassant;
    
    iss >> boardPart >> activeColor >> castling >> enPassant;
    
    int rank = 7;
    int file = 0;
    
    for (char c : boardPart) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += c - '0';
        } else {
            int index = rank * 8 + file;
            switch(c) {
                case 'P': whitePawns.set(index); break;
                case 'N': whiteKnights.set(index); break;
                case 'B': whiteBishops.set(index); break;
                case 'R': whiteRooks.set(index); break;
                case 'Q': whiteQueen.set(index); break;
                case 'K': whiteKing.set(index); break;
                case 'p': blackPawns.set(index); break;
                case 'n': blackKnights.set(index); break;
                case 'b': blackBishops.set(index); break;
                case 'r': blackRooks.set(index); break;
                case 'q': blackQueen.set(index); break;
                case 'k': blackKing.set(index); break;
            }
            file++;
        }
    }

    whiteToMove = (activeColor == "w");

    whiteCanCastleKingside = castling.find('K') != std::string::npos;
    whiteCanCastleQueenside = castling.find('Q') != std::string::npos;
    blackCanCastleKingside = castling.find('k') != std::string::npos;
    blackCanCastleQueenside = castling.find('q') != std::string::npos;

    if (enPassant == "-") {
        en_passant = -1;
    } else {
        char fileChar = enPassant[0];
        char rankChar = enPassant[1];
        en_passant = (rankChar - '1') * 8 + (fileChar - 'a');
    }

    hash = Zobrist::computeHash(*this);
}