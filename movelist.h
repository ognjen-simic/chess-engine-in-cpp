#ifndef MOVELIST_H
#define MOVELIST_H

#include "move.h"

struct MoveList{
    Move moves[256];
    Move* last;

    MoveList(){
        last = moves;
    }

    inline void add(Move m){
        *last = m;
        last++;
    }
    inline void reset(){
        last = moves;
    }

    Move* begin() {return moves;}
    Move* end() {return last;}

    int size() const {
        return static_cast<int>(last - moves);
    }
};

#endif