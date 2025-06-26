#include <bitset>
#include <iostream>
#include <string>

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


void printFullBoard(const std::bitset<64>& whitePawns, const std::bitset<64>& blackPawns,
                    const std::bitset<64>& whiteRooks, const std::bitset<64>& blackRooks,
                    const std::bitset<64>& whiteKnights, const std::bitset<64>& blackKnights,
                    const std::bitset<64>& whiteBishops, const std::bitset<64>& blackBishops,
                    const std::bitset<64>& whiteQueen, const std::bitset<64>& blackQueen,
                    const std::bitset<64>& whiteKing, const std::bitset<64>& blackKing)
{
    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; ++col) {
            int index = row * 8 + col;
            char piece = '.';

            if (whitePawns[index]) piece = 'P';
            else if (blackPawns[index]) piece = 'p';
            else if (whiteRooks[index]) piece = 'R';
            else if (blackRooks[index]) piece = 'r';
            else if (whiteKnights[index]) piece = 'N';
            else if (blackKnights[index]) piece = 'n';
            else if (whiteBishops[index]) piece = 'B';
            else if (blackBishops[index]) piece = 'b';
            else if (whiteQueen[index]) piece = 'Q';
            else if (blackQueen[index]) piece = 'q';
            else if (whiteKing[index]) piece = 'K';
            else if (blackKing[index]) piece = 'k';

            std::cout << piece;
        }
        std::cout << '\n';
    }
}

std::bitset<64> getAllPieces(
     const std::bitset<64>& whitePawns, const std::bitset<64>& whiteRooks,
    const std::bitset<64>& whiteKnights, const std::bitset<64>& whiteBishops,
    const std::bitset<64>& whiteQueen, const std::bitset<64>& whiteKing,
    const std::bitset<64>& blackPawns, const std::bitset<64>& blackRooks,
    const std::bitset<64>& blackKnights, const std::bitset<64>& blackBishops,
    const std::bitset<64>& blackQueen, const std::bitset<64>& blackKing)

    {
        return whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueen | whiteKing |
           blackPawns | blackRooks | blackKnights | blackBishops | blackQueen | blackKing;
    }


std::bitset<64> getWhitePieces(
    const std::bitset<64>& whitePawns, const std::bitset<64>& whiteRooks,
    const std::bitset<64>& whiteKnights, const std::bitset<64>& whiteBishops,
    const std::bitset<64>& whiteQueen, const std::bitset<64>& whiteKing)
    {
        return whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueen | whiteKing;
    }

std::bitset<64> getBlackPieces(
    const std::bitset<64>& blackPawns, const std::bitset<64>& blackRooks,
    const std::bitset<64>& blackKnights, const std::bitset<64>& blackBishops,
    const std::bitset<64>& blackQueen, const std::bitset<64>& blackKing)
    {
        return blackPawns | blackRooks | blackKnights | blackBishops | blackQueen | blackKing;
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

std::bitset<64> generateWhiteAttacks(std::bitset<64> whiteKnights, std::bitset<64> whitePawns, std::bitset<64> whiteBishops,
     std::bitset<64> whiteRooks, std::bitset<64> whiteQueen, std::bitset<64> whiteKing, std::bitset<64> ownPieces, std::bitset<64> allPieces,
     std::bitset<64> whitePieces, std::bitset<64> blackPieces)
{
    std::bitset<64> attacks;

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whiteKnights[sq])
        {
            attacks |= generateKnightMoves(sq);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whitePawns[sq])
        {
            attacks |= generateWhitePawnCaptures(whitePawns, blackPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whiteBishops[sq])
        {
            attacks |= generateBishopMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whiteRooks[sq])
        {
            attacks |= generateRookMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whiteQueen[sq])
        {
            attacks |= generateQueenMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (whiteKing[sq])
        {
            attacks |= generateKingMoves(sq, ownPieces, allPieces);
        }
    }

    return attacks;
}

std::bitset<64> generateBlackAttacks(std::bitset<64> blackKnights, std::bitset<64> blackPawns, std::bitset<64> blackBishops,
     std::bitset<64> blackRooks, std::bitset<64> blackQueen, std::bitset<64> blackKing, std::bitset<64> ownPieces, std::bitset<64> allPieces,
     std::bitset<64> whitePieces, std::bitset<64> blackPieces)
{
    std::bitset<64> attacks;

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackKnights[sq])
        {
            attacks |= generateKnightMoves(sq);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackPawns[sq])
        {
            attacks |= generateBlackPawnCaptures(blackPawns, whitePieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackBishops[sq])
        {
            attacks |= generateBishopMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackRooks[sq])
        {
            attacks |= generateRookMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackQueen[sq])
        {
            attacks |= generateQueenMoves(sq, ownPieces, allPieces);
        }
    }

    for (int sq = 0; sq < 64; ++sq)
    {
        if (blackKing[sq])
        {
            attacks |= generateKingMoves(sq, ownPieces, allPieces);
        }
    }

    return attacks;
}

bool whiteCanCastleKingside = true;
bool whiteCanCastleQueenside = true;
bool blackCanCastleKingside = true;
bool blackCanCastleQueenside = true;
int en_passant = -1;

bool makeMove(std::string move,
              std::bitset<64>& whiteKnights, std::bitset<64>& blackKnights,
              std::bitset<64>& whitePawns, std::bitset<64>& blackPawns,
              std::bitset<64>& whiteRooks, std::bitset<64>& blackRooks,
              std::bitset<64>& whiteBishops, std::bitset<64>& blackBishops,
              std::bitset<64>& whiteQueen, std::bitset<64>& blackQueen,
              std::bitset<64>& whiteKing, std::bitset<64>& blackKing,
              std::bitset<64>& whitePieces, std::bitset<64>& blackPieces,
              bool whiteToMove, std::bitset<64>& allPieces, std::bitset<64>& ownPieces)
{
    if (move.length() != 4) return false;

    int from = notationToIndex(move.substr(0, 2));
    int to = notationToIndex(move.substr(2, 2));
    if (from == -1 || to == -1) return false;

    if (whiteToMove)
    {
        if (whiteKnights[from])
        {
            std::bitset<64> thisKnight(1ULL << from);
            std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
            if (!legalMoves[to] || whiteKnights[to]) return false;
            if (whitePieces[to]) return false;

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whiteKnights.reset(from);
            whiteKnights.set(to);
            return true;
        }
        else if (whitePawns[from])
        {
            if (to == en_passant && to / 8 == 5)
            {
                blackPawns.reset(to - 8);
                whitePawns.reset(from);
                whitePawns.set(to);
                return true;
            }

            std::bitset<64> thisPawn(1ULL << from);
            std::bitset<64> legalMoves = generateWhitePawnMoves(thisPawn, blackPieces, allPieces);
            if (!legalMoves[to] || whitePawns[to]) return false;

            if (from / 8 == 1 && to / 8 == 3)
            {
                en_passant = from + 8;
            }
            else
            {
                en_passant = -1;
            }

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whitePawns.reset(from);
            whitePawns.set(to);
            return true;
        }
        else if (whiteRooks[from])
        {
            std::bitset<64> legalMoves = generateRookMoves(from, whitePieces, allPieces);
            if (!legalMoves[to] || whiteRooks[to]) return false;
            if (whitePieces[to]) return false;
            if (from == notationToIndex("a1")) whiteCanCastleQueenside = false;
            else if (from == notationToIndex("h1")) whiteCanCastleKingside = false;

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whiteRooks.reset(from);
            whiteRooks.set(to);
            return true;
        }
        else if (whiteBishops[from])
        {
            std::bitset<64> legalMoves = generateBishopMoves(from, whitePieces, allPieces);
            if (!legalMoves[to] || whiteBishops[to]) return false;
            if (whitePieces[to]) return false;

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whiteBishops.reset(from);
            whiteBishops.set(to);
            return true;
        }
        else if (whiteQueen[from])
        {
            std::bitset<64> legalMoves = generateQueenMoves(from, whitePieces, allPieces);
            if (!legalMoves[to] || whiteQueen[to]) return false;
            if (whitePieces[to]) return false;

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whiteQueen.reset(from);
            whiteQueen.set(to);
            return true;
        }
        else if (whiteKing[from])
        {
            if (to == notationToIndex("g1") && whiteCanCastleKingside)
            {
                if (!allPieces[notationToIndex("f1")] && !allPieces[notationToIndex("g1")])
                {
                    whiteKing.reset(from);
                    whiteKing.set(to);
                    whiteRooks.reset(notationToIndex("h1"));
                    whiteRooks.set(notationToIndex("f1"));
                    whiteCanCastleKingside = false;
                    whiteCanCastleQueenside = false;
                    return true;
                }
            }
            else if (to == notationToIndex("c1") && whiteCanCastleQueenside)
            {
                if (!allPieces[notationToIndex("b1")] && !allPieces[notationToIndex("c1")] && !allPieces[notationToIndex("d1")])
                {
                    whiteKing.reset(from);
                    whiteKing.set(to);
                    whiteRooks.reset(notationToIndex("a1"));
                    whiteRooks.set(notationToIndex("d1"));
                    whiteCanCastleKingside = false;
                    whiteCanCastleQueenside = false;
                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, whitePieces, allPieces);
            if (!legalMoves[to] || whiteKing[to]) return false;
            if (whitePieces[to]) return false;

            blackKnights.reset(to);
            blackPawns.reset(to);
            blackRooks.reset(to);
            blackBishops.reset(to);
            blackQueen.reset(to);
            blackKing.reset(to);

            whiteKing.reset(from);
            whiteKing.set(to);

            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false;

            return true;
        }
    }
    
    else 
    {
        if (blackKnights[from])
        {
            std::bitset<64> thisKnight(1ULL << from);
            std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
            if (!legalMoves[to] || blackKnights[to]) return false;
            if (blackPieces[to]) return false;

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackKnights.reset(from);
            blackKnights.set(to);
            return true;
        }
        else if (blackPawns[from])
        {
            if (to == en_passant && to / 8 == 2)
            {
                whitePawns.reset(to + 8);
                blackPawns.reset(from);
                blackPawns.set(to);
            }

            std::bitset<64> thisPawn(1ULL << from);
            std::bitset<64> legalMoves = generateBlackPawnMoves(thisPawn, whitePieces, allPieces);
            if (!legalMoves[to] || blackPawns[to]) return false;

            if (from / 8 == 6 && to / 8 == 4)
            {
                en_passant = from - 8;
            }
            else
            {
                en_passant = -1;
            }

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackPawns.reset(from);
            blackPawns.set(to);
            return true;
        }
        else if (blackRooks[from])
        {
            std::bitset<64> legalMoves = generateRookMoves(from, blackPieces, allPieces);
            if (!legalMoves[to] || blackRooks[to]) return false;
            if (blackPieces[to]) return false;
            if (from == notationToIndex("a8")) blackCanCastleQueenside = false;
            else if (from == notationToIndex("h8")) blackCanCastleKingside = false;

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackRooks.reset(from);
            blackRooks.set(to);
            return true;
        }
        else if (blackBishops[from])
        {
            std::bitset<64> legalMoves = generateBishopMoves(from, blackPieces, allPieces);
            if (!legalMoves[to] || blackBishops[to]) return false;
            if (blackPieces[to]) return false;

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackBishops.reset(from);
            blackBishops.set(to);
            return true;
        }
        else if (blackQueen[from])
        {
            std::bitset<64> legalMoves = generateQueenMoves(from, blackPieces, allPieces);
            if (!legalMoves[to] || blackQueen[to]) return false;
            if (blackPieces[to]) return false;

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackQueen.reset(from);
            blackQueen.set(to);
            return true;
        }
        else if (blackKing[from])
        {
            if (to ==  notationToIndex("g8") && blackCanCastleKingside)
            {
                if (!allPieces[notationToIndex("f8")] && !allPieces[notationToIndex("g8")])
                {
                    blackKing.reset(from);
                    blackKing.set(to);
                    blackRooks.reset(notationToIndex("h8"));
                    blackRooks.set(notationToIndex("f8"));
                    blackCanCastleKingside = false;
                    blackCanCastleQueenside = false;

                    return true;
                }
            }
            else if (to ==  notationToIndex("c8") && blackCanCastleQueenside)
            {
                if (!allPieces[notationToIndex("b8")] && !allPieces[notationToIndex("c8")] && !allPieces[notationToIndex("d8")])
                {
                    blackKing.reset(from);
                    blackKing.set(to);
                    blackRooks.reset(notationToIndex("a8"));
                    blackRooks.set(notationToIndex("d8"));
                    blackCanCastleKingside = false;
                    blackCanCastleQueenside = false;

                    return true;
                }
            }

            std::bitset<64> legalMoves = generateKingMoves(from, blackPieces, allPieces) & ~generateWhiteAttacks(whiteKnights, whitePawns,
                 whiteBishops, whiteRooks, whiteQueen, whiteKing, ownPieces, allPieces, whitePieces, blackPieces);
            if (!legalMoves[to] || blackKing[to]) return false;
            if (blackPieces[to]) return false;

            whiteKnights.reset(to);
            whitePawns.reset(to);
            whiteRooks.reset(to);
            whiteBishops.reset(to);
            whiteQueen.reset(to);
            whiteKing.reset(to);
  
            blackKing.reset(from);
            blackKing.set(to);

            blackCanCastleKingside = false;
            blackCanCastleQueenside = false;

            return true;
        }
    }


    return false;
}

int main()
{
    std::bitset<64> whitePawns(0x000000000000FF00);
    std::bitset<64> whiteRooks(1ULL << 0 | 1ULL << 7);
    std::bitset<64> whiteKnights(1ULL << 1 | 1ULL << 6);
    std::bitset<64> whiteBishops(1ULL << 2 | 1ULL << 5);
    std::bitset<64> whiteQueen(1ULL << 3);
    std::bitset<64> whiteKing(1ULL << 4);

    std::bitset<64> blackPawns(0x00FF000000000000);
    std::bitset<64> blackRooks(1ULL << 56 | 1ULL << 63);
    std::bitset<64> blackKnights(1ULL << 57 | 1ULL << 62);
    std::bitset<64> blackBishops(1ULL << 58 | 1ULL << 61);
    std::bitset<64> blackQueen(1ULL << 59);
    std::bitset<64> blackKing(1ULL << 60);

    std::bitset<64> allPieces = getAllPieces(  whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing,
    blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);
    std::bitset<64> whitePieces = getWhitePieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing);
    std::bitset<64> blackPieces = getBlackPieces(blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);

    bool whiteToMove = true;

    std::bitset<64> ownPieces;
    if (whiteToMove) ownPieces = getWhitePieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing);
    else ownPieces = getBlackPieces(blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);

    std::string move;

    while (true)
    {
        printFullBoard(whitePawns, blackPawns, whiteRooks, blackRooks,
               whiteKnights, blackKnights, whiteBishops, blackBishops,
               whiteQueen, blackQueen, whiteKing, blackKing);

        std::cout << (whiteToMove ? "White" : "Black") << " to move:\n";
        std::cin >> move;

        if (move == "exit") break;

        bool success = makeMove(move, whiteKnights, blackKnights, whitePawns, blackPawns,
                        whiteRooks, blackRooks, whiteBishops, blackBishops,
                        whiteQueen, blackQueen, whiteKing, blackKing, whitePieces, blackPieces,
                        whiteToMove, allPieces, ownPieces);

        if (success)
        {
            whitePieces = getWhitePieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing);
            blackPieces = getBlackPieces(blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);
            allPieces = getAllPieces(whitePawns, whiteRooks, whiteKnights, whiteBishops, whiteQueen, whiteKing,
                              blackPawns, blackRooks, blackKnights, blackBishops, blackQueen, blackKing);

            whiteToMove = !whiteToMove;
            std::cout << "successful move\n";
        }
        else
        {
            std::cout << "Invalid move!\n";
        }
    }

    return 0;
}