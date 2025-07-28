#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <bitset>
#include <random>
#include <cstdint>
#include "board.h"

namespace Zobrist
{
    extern uint64_t pieceKeys[12][64];
    extern uint64_t castlingKeys[16];
    extern uint64_t en_passantKeys[8];
    extern uint64_t sideKey;

    void initializeKeys();
    uint64_t computeHash(const Board& board);
}

#endif