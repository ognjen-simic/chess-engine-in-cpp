#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <string>

typedef uint16_t Move;

namespace MoveEncoder
{
    inline Move encode(int from, int to, int flag)
    {
        return from | (to << 6) | (flag << 12);
    }

    inline int getFrom(Move m)
    {
        return m & 0x3F;
    }
    inline int getTo(Move m)
    {
        return (m >> 6) & 0x3F;
    }
    inline int getFlag(Move m)
    {
        return (m >> 12) & 0xF;
    }

    const int QUIET = 0;
    const int PROMO_KNIGHT = 1;
    const int PROMO_BISHOP = 2;
    const int PROMO_ROOK = 3;
    const int PROMO_QUEEN = 4;
    const int EN_PASSANT = 5;
}
#endif