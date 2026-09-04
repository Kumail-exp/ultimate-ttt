#pragma once
#include "Board.hpp"
#include <unordered_map>

enum Flag{
     EXACT,LOWER_BOUND,UPPER_BOUND
};

struct TTEntry{
    float eval;
    int depth;
    uint32_t hash;
    Flag flag;
};

class Transpositiontable{
private:
    std::unordered_map<uint32_t, TTEntry> table;
    static const int MAX_ENTRIES = 10000000;
    
public:
    uint32_t hash_board(Board b) {
        uint32_t h = 0;
        for(int i = 0; i < 9; i++) {
            h ^= b.board[i].value;
        }
        //am i this dumb forgot to add following board details 
        h ^= (uint32_t)b.Aturn;
        h ^= (uint32_t)b.nextBig*31;
        h ^= (uint32_t)b.freemove*97;
        return h;
    }
    
    bool lookup(Board b, int depth, float& result, float alpha, float beta) {
        uint32_t h = hash_board(b);
        auto it = table.find(h);
        
        if(it != table.end() && it->second.depth >= depth) {
            if(it->second.flag == EXACT) {
                result = it->second.eval;
                return true;
            }
            //i hate alpha beta pruning so much so much this would just have been such a easy to grab from internet
            if(it->second.flag == LOWER_BOUND && it->second.eval > alpha) {
                alpha = it->second.eval;
            }
            if(it->second.flag == UPPER_BOUND && it->second.eval < beta) {
                beta = it->second.eval;
            }
            if(alpha >= beta) {
                result = it->second.eval;
                return true;
            }
        }
        return false;
    }
    
    void store(Board b, int depth, float eval, float alpha, float beta) {
        if(table.size() >= MAX_ENTRIES) {
            table.clear();
        }
        uint32_t h = hash_board(b);
        
        Flag f;
        if(eval <= alpha) {
            f = UPPER_BOUND;
        } else if(eval >= beta) {
            f = LOWER_BOUND;
        } else {
            f = EXACT;
        }
        
        table[h] = {eval, depth, h, f};
    }
    
    void clear() {
        table.clear();
    }
};

Transpositiontable tt;

const float weights[9]={1.44, 1,  1.44,
                1,  1.71,   1,
                1.44,   1,  1.44};
const float cellWin_weight=50;

Move relToUni(Move m){
    return {(m.bigidx/3)*3+(m.smallidx/3),(m.bigidx%3)*3+(m.smallidx%3)};
}

float Eval(Board b) {
    float cached;
    if (tt.lookup(b, 0, cached, -1e9, 1e9)) {
        return cached;
    }
    //love you sometimes copilot atleast this code is something i can take from you
    //dont think that u are anymore helpfull to me fuckk off
    static const float weights[9] = {
        1.44f, 1.0f, 1.44f,
        1.0f,  1.71f, 1.0f,
        1.44f, 1.0f, 1.44f
    };

    float sval = 0.0f;

    for (int i = 0; i < 9; i++) {
        int e = b.winners.cellAt(i);
        if (e == 1) sval += 50.0f * weights[i];
        if (e == 2) sval -= 50.0f * weights[i];
    }

    for (int r = 0; r < 3; r++) {
        int x = 0, o = 0, empty = 0;
        for (int c = 0; c < 3; c++) {
            int e = b.winners.cellAt(r * 3 + c);
            if (e == 1) x++;
            else if (e == 2) o++;
            else empty++;
        }
        if (o == 0) {
            if (x == 2) sval += 28.0f;
            else if (x == 1) sval += 5.0f;
        }
        if (x == 0) {
            if (o == 2) sval -= 28.0f;
            else if (o == 1) sval -= 5.0f;
        }
    }

    // columns
    for (int c = 0; c < 3; c++) {
        int x = 0, o = 0, empty = 0;
        for (int r = 0; r < 3; r++) {
            int e = b.winners.cellAt(r * 3 + c);
            if (e == 1) x++;
            else if (e == 2) o++;
            else empty++;
        }
        if (o == 0) {
            if (x == 2) sval += 28.0f;
            else if (x == 1) sval += 5.0f;
        }
        if (x == 0) {
            if (o == 2) sval -= 28.0f;
            else if (o == 1) sval -= 5.0f;
        }
    }

    {
        int x = 0, o = 0, empty = 0;
        for (int i = 0; i < 3; i++) {
            int e = b.winners.cellAt(i * 3 + i);
            if (e == 1) x++;
            else if (e == 2) o++;
            else empty++;
        }
        if (o == 0) {
            if (x == 2) sval += 28.0f;
            else if (x == 1) sval += 5.0f;
        }
        if (x == 0) {
            if (o == 2) sval -= 28.0f;
            else if (o == 1) sval -= 5.0f;
        }
    }

    {
        int x = 0, o = 0, empty = 0;
        for (int i = 0; i < 3; i++) {
            int e = b.winners.cellAt(i * 3 + (2 - i));
            if (e == 1) x++;
            else if (e == 2) o++;
            else empty++;
        }
        if (o == 0) {
            if (x == 2) sval += 28.0f;
            else if (x == 1) sval += 5.0f;
        }
        if (x == 0) {
            if (o == 2) sval -= 28.0f;
            else if (o == 1) sval -= 5.0f;
        }
    }

    tt.store(b, 0, sval, -1e9, 1e9);
    return sval;
}