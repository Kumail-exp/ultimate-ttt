#pragma once
#include "Move.hpp"
#include "Board.hpp"

//ts is for small board
float MOVE_IMPORTANCE[9]={1.44,1.7,1.44,1.7,1,1.7,1.44,1.7,1.44};

static constexpr double LOCAL_POS[9] ={1.44, 1,  1.44,
                1,  1.71,   1,
                1.44,   1,  1.44};

static constexpr double GLOBAL_WEIGHT[9] = {1.44, 1,  1.44,
                1,  1.81,   1,
                1.44,   1,  1.44};

// classic 9-bit win masks (one bit per cell)
static const int WIN9[8] = {
    0b000000111, // 0 1 2
    0b000111000, // 3 4 5
    0b111000000, // 6 7 8
    0b001001001, // 0 3 6
    0b010010010, // 1 4 7
    0b100100100, // 2 5 8
    0b100010001, // 0 4 8
    0b001010100  // 2 4 6
};
int POS_SCORE[512];

void initPosScore() {
    for (int bits = 0; bits < 512; ++bits) {
        int score = 0;
        for (int i = 0; i < 9; ++i) {
            if (bits & (1 << i))
                score += LOCAL_POS[i];
        }
        POS_SCORE[bits] = score;
    }
}
void Move_ordering(std::vector<Move>& legals,int small_killer){
    //who better than the our beloved insertion sort
    auto importance = [small_killer](const Move& move){
        if (move.smallidx == small_killer)
            return 1000000.0f; //killer move gets top ahh priority
        return MOVE_IMPORTANCE[move.smallidx];
    };
    for (size_t i = 1; i < legals.size(); ++i){
        Move key = legals[i];
        size_t j = i;
        while (j > 0 && importance(legals[j - 1]) < importance(key)){
            legals[j] = legals[j - 1];
            --j;
        }
        legals[j] = key;
    }
}
inline int evaluateMeta(const Board& b) {
    int xBits = 0;
    int oBits = 0;
    for (int g=0; g < 9; ++g) {
        int w =(b.meta >> (2 * g)) & 3;
        if(w == 1)
            xBits |=(1 << g);
        else if(w==2)
            oBits|=(1 << g);
    }

    int score = 0;
    score += POS_SCORE[xBits];
    score -= POS_SCORE[oBits];
    for (int mask : WIN9){
        //i could have just copy pasted it from below i am dumb
        int x = __builtin_popcount(xBits & mask);
        int o = __builtin_popcount(oBits & mask);
        if (x == 2 && o == 0)
            score += 1000;
        else if (x == 1 && o == 0)
            score += 50;
        if (o == 2 && x == 0)
            score -= 1000;
        else if (o == 1 && x == 0)
            score -= 50;
    }
    return score;
}
//extract the funky ahh from the bits
inline void extractBits(uint32_t s, int& xBits, int& oBits) {
    xBits = oBits = 0;
    for (int i = 0; i < 9; ++i) {
        int cell = (s>>(2*i))&3;
        if(cell==1){
            xBits|=(1<<i);
        }else{
            if(cell==2){
                 oBits|=(1<<i);
            }
        }
    }
}

inline int evaluateLocal(uint32_t s, bool meIsX){
    int xBits, oBits;
    extractBits(s, xBits, oBits);
    int myBits  = meIsX ? xBits : oBits;
    int oppBits = meIsX ? oBits : xBits;
    int score = POS_SCORE[myBits] - POS_SCORE[oppBits];
    // threats
    for (int mask:WIN9){
        int my  = __builtin_popcount(myBits  & mask);
        int opp = __builtin_popcount(oppBits & mask);

        if (my == 2 && opp == 0)
            score += 28;
        else if (my == 1 && opp == 0)
            score += 5;
        if (opp == 2 && my == 0)
            score -= 28;
        else if (opp == 1 && my == 0)
            score -= 5;
    }

    return score;
}
inline  float Eval(const Board& b) {
    //relative to only x perspective unlike zammy
    float score = evaluateMeta(b);
    for (int g = 0; g < 9; ++g) {
        int w =((b.meta >> (2 * g)) & 3);
        int weight = GLOBAL_WEIGHT[g];
        if(w==1){
            score+=weight*100.0f;
        }else if (w == 2){
            score-=weight*100.0f;
        }else if (w==0){
            //local threats idea was mine tbh i recommended zammu to add it
            score += evaluateLocal(b.small[g], true) * weight;
        }
    }
    return score;
}