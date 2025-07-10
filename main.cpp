#include <bitset>
#include <iostream>
#include <string>
#include <map>
#include "uci.h"

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
    bool whiteToMove = true;
    int en_passant = -1;

    std::bitset<64> getWhitePieces() const
    {
        return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
    }

    std::bitset<64> getBlackPieces() const
    {
        return blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing;
    }

    std::bitset<64> getAllPieces() const
    {
        return getWhitePieces() | getBlackPieces();
    }

    std::bitset<64> getOwnPieces(bool whiteToMove) const
    {
        return whiteToMove 
        ? whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing
        : blackPawns | blackKnights | blackBishops |blackRooks | blackQueen | blackKing;
    }

    std::bitset<64> getOpponentPieces(bool whiteToMove) const {
    return whiteToMove
        ? blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing
        : whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
}
};

void printPawns(std::bitset<64>& whitePawns, std::bitset<64>& blackPawns)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whitePawns[index]) std::cout << "P";
            else if (blackPawns[index]) std::cout << "p";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


void printRooks(std::bitset<64>& whiteRooks, std::bitset<64>& blackRooks)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whiteRooks[index]) std::cout << "R";
            else if (blackRooks[index]) std::cout << "r";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


void printKnights(std::bitset<64>& whiteKnights, std::bitset<64>& blackKnights)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whiteKnights[index]) std::cout << "N";
            else if (blackKnights[index]) std::cout << "n";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


void printBishops(std::bitset<64>& whiteBishops, std::bitset<64>& blackBishops)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whiteBishops[index]) std::cout << "B";
            else if (blackBishops[index]) std::cout << "b";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


void printQueen(std::bitset<64>& whiteQueen, std::bitset<64>& blackQueen)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whiteQueen[index]) std::cout << "Q";
            else if (blackQueen[index]) std::cout << "q";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


void printKing(std::bitset<64>& whiteKing, std::bitset<64>& blackKing)
{
    for (int row = 7; row >= 0; --row)
    {
        for (int col = 0; col < 8; ++col)
        {
            int index = row * 8 + col;
            if (whiteKing[index]) std::cout << "K";
            else if (blackKing[index]) std::cout << "k";
            else std::cout << ".";
        }
        std::cout << "\n";
    }
}


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
            if (!legalMoves[to] || board.whiteKnights[to]) return false;
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
            if (!legalMoves[to] || board.whitePawns[to]) return false;

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

            return true;
        }
        else if (board.whiteRooks[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateRookMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to] || board.whiteRooks[to]) return false;
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

            return true;
        }
        else if (board.whiteBishops[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateBishopMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to] || board.whiteBishops[to]) return false;
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

            return true;
        }
        else if (board.whiteQueen[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateQueenMoves(from, board.getWhitePieces(), board.getAllPieces());
            if (!legalMoves[to] || board.whiteQueen[to]) return false;
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

                    if (isKingInCheck(newBoard) || generateBlackAttacks(board)[notationToIndex("f1")]) return false;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.whiteCanCastleKingside = false;
                    board.whiteCanCastleQueenside = false;
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

                    if (isKingInCheck(newBoard) || generateBlackAttacks(board)[notationToIndex("d1")]) return false;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.whiteCanCastleKingside = false;
                    board.whiteCanCastleQueenside = false;
                    return true;

                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, board.getWhitePieces(), board.getAllPieces()) & ~generateBlackAttacks(board);
            if (!legalMoves[to] || board.whiteKing[to]) return false;
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
            if (!legalMoves[to] || board.blackKnights[to]) return false;
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

            return true;
        }
        else if (board.blackPawns[from])
        {
            if (to == board.en_passant && to / 8 == 2)
            {
                Board newBoard = board;
        
                newBoard.whitePawns.reset(to + 8);
                newBoard.blackPawns.reset(from);
                newBoard.blackPawns.set(to);

                if (isKingInCheck(newBoard)) return false;
                board = newBoard;
                board.whiteToMove = !board.whiteToMove;
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
            if (!legalMoves[to] || board.blackPawns[to]) return false;

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

            return true;
        }
        else if (board.blackRooks[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateRookMoves(from, board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to] || board.blackRooks[to]) return false;
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

            return true;
        }
        else if (board.blackBishops[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateBishopMoves(from, board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to] || board.getBlackPieces()[to]) return false;
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

            return true;
        }
        else if (board.blackQueen[from])
        {
            board.en_passant = -1;

            std::bitset<64> legalMoves = generateQueenMoves(from,board.getBlackPieces(), board.getAllPieces());
            if (!legalMoves[to] || board.blackQueen[to]) return false;
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

                    if (isKingInCheck(newBoard) || generateWhiteAttacks(board)[notationToIndex("f8")]) return false;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.blackCanCastleKingside = false;
                    board.blackCanCastleQueenside = false;
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

                    if (isKingInCheck(newBoard) || generateWhiteAttacks(board)[notationToIndex("d8")]) return false;
                    board = newBoard;
                    board.whiteToMove = !board.whiteToMove;
                    board.blackCanCastleKingside = false;
                    board.blackCanCastleQueenside = false;
                    return true;

                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, board.getBlackPieces(), board.getAllPieces()) & ~generateWhiteAttacks(board);
            if (!legalMoves[to] || board.blackKing[to]) return false;
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

            return true;
        }
    }


    return false;
}

int evaluatePosition(
    const std::bitset<64>& whitePawns,
    const std::bitset<64>& blackPawns,
    const std::bitset<64>& whiteKnights,
    const std::bitset<64>& blackKnights,
    const std::bitset<64>& whiteBishops,
    const std::bitset<64>& blackBishops,
    const std::bitset<64>& whiteRooks,
    const std::bitset<64>& blackRooks,
    const std::bitset<64>& whiteQueen,
    const std::bitset<64>& blackQueen
) {
    int score = 0;

    score += whitePawns.count() * 100;
    score -= blackPawns.count() * 100;

    score += whiteKnights.count() * 320;
    score -= blackKnights.count() * 320;

    score += whiteBishops.count() * 330;
    score -= blackBishops.count() * 330;

    score += whiteRooks.count() * 500;
    score -= blackRooks.count() * 500;

    score += whiteQueen.count() * 900;
    score -= blackQueen.count() * 900;

    return score;
}

int main()
{
    uciLoop();
    return 0;
}