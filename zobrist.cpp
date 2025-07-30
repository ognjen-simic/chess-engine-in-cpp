#include "zobrist.h"

namespace Zobrist
{
    uint64_t pieceKeys[12][64];
    uint64_t castlingKeys[16];
    uint64_t en_passantKeys[8];
    uint64_t sideKey;

    uint64_t generateRandom64()
    {
        static std::mt19937_64 rng(0xABCDEF);
        static std::uniform_int_distribution<uint64_t> dist;

        return dist(rng);
    }

    void initializeKeys()
    {
        for (int piece = 0; piece < 12; ++piece)
        {
            for (int square = 0; square < 64; ++square)
            {
                pieceKeys[piece][square] = generateRandom64();
            }
        }

        for (int i = 0; i < 16; ++i)
        {
            castlingKeys[i] = generateRandom64();
        }

        for (int file = 0; file < 8; ++file)
        {
            en_passantKeys[file] = generateRandom64();
        }

        sideKey = generateRandom64();
    }

    uint64_t computeHash(const Board& board)
    {
        uint64_t hash = 0ULL;

        for (int sq = 0; sq < 64; ++sq)
        {
            if (board.whitePawns[sq])   hash ^= pieceKeys[0][sq];
            if (board.whiteKnights[sq]) hash ^= pieceKeys[1][sq];
            if (board.whiteBishops[sq]) hash ^= pieceKeys[2][sq];
            if (board.whiteRooks[sq])   hash ^= pieceKeys[3][sq];
            if (board.whiteQueen[sq])   hash ^= pieceKeys[4][sq];
            if (board.whiteKing[sq])    hash ^= pieceKeys[5][sq];

            if (board.blackPawns[sq])   hash ^= pieceKeys[6][sq];
            if (board.blackKnights[sq]) hash ^= pieceKeys[7][sq];
            if (board.blackBishops[sq]) hash ^= pieceKeys[8][sq];
            if (board.blackRooks[sq])   hash ^= pieceKeys[9][sq];
            if (board.blackQueen[sq])   hash ^= pieceKeys[10][sq];
            if (board.blackKing[sq])    hash ^= pieceKeys[11][sq];
        }

        int castlingRights = 0;
        if (board.whiteCanCastleKingside)  castlingRights |= 1 << 0;
        if (board.whiteCanCastleQueenside) castlingRights |= 1 << 1;
        if (board.blackCanCastleKingside)  castlingRights |= 1 << 2;
        if (board.blackCanCastleQueenside) castlingRights |= 1 << 3;
        hash ^= castlingKeys[castlingRights];

        if (board.en_passant >= 0 && board.en_passant < 64)
        {
            int file = board.en_passant % 8;
            hash ^= en_passantKeys[file];
        }

        if (!board.whiteToMove)
            hash ^= sideKey;

        return hash;
    }
}