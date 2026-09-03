#pragma once
#include "Board.hpp"
#include "Eval.hpp"
#include <cmath>


float minmax(Board b,int depth,bool maximising){
    int w=b.winnercheck();
    if(w!=0){
        return INFINITY*(w==1?1:-1);
    }
    if(depth==0){
        return Eval(b);
    }
    if(maximising){
        float max_eval=-INFINITY;
        int rtg=b.nextBig;
        for(Move x: b.legalMoves()){
            b.move(x.smallidx,x.bigidx);
            float eval = minmax(b, depth - 1, false);
            b.pop(x.smallidx,x.bigidx,rtg);
            if(eval>max_eval){
                max_eval=eval;
            }
        }
        return max_eval;
    }else{
        float min_eval=INFINITY;
        int rtg=b.nextBig;
        for(Move x: b.legalMoves()){
            b.move(x.smallidx,x.bigidx);
            float eval = minmax(b, depth - 1, true);
            b.pop(x.smallidx,x.bigidx,rtg);
            if(eval<min_eval){
                min_eval=eval;
            }
        }
        return min_eval;
    }
}
struct Line{
    Move move;
    float eval;
};
Line best_move(int depth, bool maximising, Board b) {
    Line best = {{-1,-1}, maximising ? -INFINITY : INFINITY};
    int rtg=b.nextBig;
    for (Move x : b.legalMoves()) {
        b.move(x.smallidx, x.bigidx);
        float result = minmax(b, depth - 1, !maximising);
        b.pop(x.smallidx, x.bigidx,rtg);
        if (maximising) {
            if (result > best.eval)
                best = {x, result};
        } else {
            if (result < best.eval)
                best = {x, result};
        }
    }

    return best;
}