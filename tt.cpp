#include "tt.h"

constexpr size_t TT_SIZE = 1 << 20; 

static TTEntry ttTable[TT_SIZE];

inline size_t ttIndex(uint64_t key)
{
    return key & (TT_SIZE - 1);
}

void storeTT(uint64_t key, int depth, int score, TTFlag flag, const std::string& bestMove)
{
    size_t index = ttIndex(key);
    TTEntry& entry = ttTable[index];

    if (entry.key == 0 || depth >= entry.depth)
    {
        entry.key = key;
        entry.depth = depth;
        entry.score = score;
        entry.flag = flag;
        entry.bestmove = bestMove;
    }
}

bool probeTT(uint64_t key, int depth, int alpha, int beta, int& score, std::string& bestMove)
{
    size_t index = ttIndex(key);
    TTEntry& entry = ttTable[index];

    if (entry.key == key)
    {
        bestMove = entry.bestmove;

        if (entry.depth >= depth)
        {
            switch (entry.flag)
            {
                case EXACT:
                    score = entry.score;
                    return true;
                case LOWERBOUND:
                    if (entry.score >= beta)
                    {
                        score = entry.score;
                        return true;
                    }
                    break;
                case UPPERBOUND:
                    if (entry.score <= alpha)
                    {
                        score = entry.score;
                        return true;
                    }
                    break;
            }
        }
    }

    return false;
}
