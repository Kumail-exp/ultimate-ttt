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
float evalCell(int bigidx,Board b){
    int x[3]={0},o[3]={0};
    //row 
    for(int i=0;i<3;i++){
        int oc=0,xc=0;
        for(int j=0;j<3;j++){
            int e=b.get(relToUni({3*i+j,bigidx}));
            oc+=e==2?1:0;
            xc+=e==1?1:0;
        }
        if(oc==0){x[xc]++;}
        if(xc==0){o[oc]++;}

        oc=0,xc=0;
        for(int j=0;j<3;j++){
            int e=b.get(relToUni({3*j+i,bigidx}));
            oc+=e==2?1:0;
            xc+=e==1?1:0;
        }
        if(oc==0){x[xc]++;}
        if(xc==0){o[oc]++;}
    }
    int oc=0,xc=0;
    for(int i=0;i<3;i++){
        int e=b.get(relToUni({4*i,bigidx}));
        oc+=e==2?1:0;
        xc+=e==1?1:0;
    }
    if(oc==0){x[xc]++;}
    if(xc==0){o[oc]++;}

    oc=0,xc=0;
    for(int i=0;i<3;i++){
        int e=b.get(relToUni({2*i+2,bigidx}));
        oc+=e==2?1:0;
        xc+=e==1?1:0;
    }
    if(oc==0){x[xc]++;}
    if(xc==0){o[oc]++;}

    return (7*x[2]+2*x[1])-(7*o[2]+2*o[1]);
}
float Eval(Board b){
    float cached;
    if(tt.lookup(b, 0, cached, -1e9, 1e9)) {
        return cached;
    }
    
    //assuming the board hasnt been won yet
    float sval=0.0;
    for(int i=0;i<9;i++){
        float val=0.0;
        if(b.winners.cellAt(i)!=0){
            val+=cellWin_weight*weights[i]*(-2*b.winners.cellAt(i)+3);
        }else{
            val+=evalCell(i,b);
        }
        sval+=val*weights[i];
    }
    
    tt.store(b, 0, sval, -1e9, 1e9);
    return sval;
}