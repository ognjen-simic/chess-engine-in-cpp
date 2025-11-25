#ifndef TT_H
#define TT_H

#include <cstdint>
#include <iostream>

enum TTFlag
{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry
{
    uint64_t key = 0;
    int depth = 0;
    int score = 0;
    TTFlag flag;
    uint16_t bestmove = 0;
};

void storeTT(uint64_t key, int depth, int score, TTFlag flag, const uint16_t& bestMove);
bool probeTT(uint64_t key, int depth, int alpha, int beta, int& score, uint16_t& bestMove);

#endif