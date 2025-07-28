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
    std::string bestmove = "";
};

void storeTT(uint64_t key, int depth, int score, TTFlag flag, std::string bestMove);
bool probeTT(uint64_t key, int depth, int alpha, int beta, int& score, std::string& bestMove);

#endif