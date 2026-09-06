#pragma once
#include <cstdint>
#include <random>
#include "Move.hpp"
struct Zobrist {
    
    uint64_t cell[9][9][3];
    uint64_t meta[9][3];//meta winners
    uint64_t next[10];
    uint64_t side;


    //to anyone every reading my codebase yes i stole this from internet and also there is no stealing if the property is public
    Zobrist() {
        std::mt19937_64 rng(0xC0FFEE);
        std::uniform_int_distribution<uint64_t> dist;

        for (int b = 0; b < 9; ++b)
            for (int s = 0; s < 9; ++s)
                for (int p = 1; p <= 2; ++p)
                    cell[b][s][p] = dist(rng);

        for (int b = 0; b < 9; ++b)
            for (int p = 1; p <= 2; ++p)
                meta[b][p] = dist(rng);

        for (int i = 0; i < 10; ++i)
            next[i] = dist(rng);

        side = dist(rng);
    }
};

inline Zobrist& zobrist() {
    static Zobrist z;
    return z;
}

static constexpr int TT_SIZE = 1 << 22;
static constexpr int TT_MASK = TT_SIZE - 1;

enum TTFlag : uint8_t { TT_EXACT = 0, TT_LOWER = 1, TT_UPPER = 2 };

struct TTEntry {
    uint64_t key;
    int16_t  eval;//keeping in mind the win scores
    int8_t   depth;
    uint8_t  flag;
    uint8_t  move;//big*9 + small or 255 is none
};

inline TTEntry* tt() {
    static TTEntry table[TT_SIZE];//not more than 100mb
    return table;
}

inline void ttStore(uint64_t key, int depth, double score,TTFlag flag,Move m){
    TTEntry& e = tt()[key & TT_MASK];
    if (e.key == key && e.depth > depth) return;

    e.key   = key;
    e.depth = (int8_t)depth;
    //overflow in mind
    if (score >  30000) score =  30000;
    if (score < -30000) score = -30000;
    e.eval  = (int16_t)score;
    e.flag  = flag;
    e.move  = (m.bigidx <= 8 && m.smallidx <= 8)?(uint8_t)(m.bigidx * 9 + m.smallidx):255;
}

inline bool ttProbe(uint64_t key, int depth, double alpha, double beta,double& outScore, Move& outMove){
    const TTEntry& e = tt()[key & TT_MASK];
    if (e.key != key) return false;

    outMove = (e.move == 255)? Move{9,9}:Move{uint8_t(e.move % 9), uint8_t(e.move / 9)};

    if (e.depth < depth) return false;//if i ever add move ordering

    double s = e.eval;
    if (e.flag == TT_EXACT) {
        outScore = s;
        return true;
    }
    if (e.flag == TT_LOWER && s > alpha) alpha = s;
    if (e.flag == TT_UPPER && s < beta)  beta  = s;
    if (alpha >= beta) {
        outScore = s;
        return true;
    }
    return false;//only move is useful
}
