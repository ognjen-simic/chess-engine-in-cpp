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

bool makeMove(std::string move, std::bitset<64>& whiteKnights, std::bitset<64>& blackKnights, bool whiteToMove)
{
    if (move.length() != 4) return false;

    int from = notationToIndex(move.substr(0, 2));
    int to = notationToIndex(move.substr(2, 2));
    if (from == -1 || to == -1) return false;

    if (whiteToMove)
    {
        if (!whiteKnights[from]) return false;

        std::bitset<64> thisKnight(1ULL << from);
        std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
        if (!legalMoves[to] || whiteKnights[to]) return false;

        blackKnights.reset(to);

        whiteKnights.reset(from);
        whiteKnights.set(to);
        return true;
    }
    else
    {
        if (!blackKnights[from]) return false;

        std::bitset<64> thisKnight(1ULL << from);
        std::bitset<64> legalMoves = generateKnightMoves(thisKnight);
        if (!legalMoves[to] || blackKnights[to]) return false;

        whiteKnights.reset(to);

        blackKnights.reset(from);
        blackKnights.set(to);
        return true;
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

    bool whiteToMove = true;

    std::string move;

    while (true)
    {
        printKnights(whiteKnights, blackKnights);

        std::cout << (whiteToMove ? "White" : "Black") << " to move:\n";
        std::cin >> move;

        if (move == "exit") break;

        bool success = makeMove(move, whiteKnights, blackKnights, whiteToMove);

        if (success)
        {
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