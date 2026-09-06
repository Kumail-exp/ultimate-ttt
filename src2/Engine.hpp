#pragma once
#include "Board.hpp"
#include "Eval.hpp"
#include <cmath>
#include <vector>
#include <chrono>
double minmax(Board& b, int depth, bool maximising,double alpha, double beta) {
    

    //winner type shift
    if (b.winner == 1) return  1e9;
    if (b.winner == 2) return -1e9;
    if (b.winner == 3) return  0.0;

    std::vector<Move> moves;
    b.legalMoves(moves);
    if (moves.empty()) {
        return 0.0;//drawwww
    }
    if(depth == 0){
        return Eval(b);
    }

    if(maximising){
        double best= -INFINITY;
        for(Move x : moves){
            Board::Undo u;
            b.make(x,u);
            double eval = minmax(b,depth-1,false,alpha,beta);
            b.unmake(u);
            if(eval > best){
                best =eval;
            }
            if(eval>alpha){
                alpha=eval;
            }
            if(beta <= alpha){
                break;
            }
        }
        return best;
    } else {
        double best = +INFINITY;
        for(Move x : moves){
            Board::Undo u;
            b.make(x,u);
            double eval=minmax(b,depth-1,true,alpha,beta);
            b.unmake(u);
            if(eval < best){
                best = eval;
            }
            if(eval<beta){
                beta=eval;
            }
            if(beta <= alpha){
                break;
            }
        }
        return best;
    }
}

struct Line{
    Move m;
    double eval;
    int depth;
};
Line best_move(int depth, bool maximising, Board b) {
    if (b.winner != 0) {
        return {{9, 9},0.0,depth};//ts already over
    }

    std::vector<Move> moves;
    b.legalMoves(moves);
    if(moves.empty()){
        return {{9, 9},0.0,depth};
    }

    Line best = {moves[0], maximising?-1e9:1e9,depth};
    bool first=true;

    for(Move x :moves){
        Board::Undo u;
        b.make(x,u);
        double result =minmax(b,depth-1,!maximising,-INFINITY,INFINITY);
        b.unmake(u);
        if(first){
            best ={x,result,depth};
            first = false;
        }else if(maximising){
            if(result > best.eval){ 
                best = {x, result, depth};
            }
        }else{
            if(result <best.eval){ 
                best = {x, result, depth};
            }
        }
    }
    return best;
}
Line tbest_move(float time, bool maximising, Board b){
    //highly inspired from zammus design
    using Clock = std::chrono::high_resolution_clock;
    using sec = std::chrono::duration<double>;
    auto start= Clock::now();
    auto softEnd= start + sec(time * 0.85);
    auto hardEnd= start + sec(time * 0.96);

    Line best = {{9, 9}, 0.0, 0};
    double lastDepthTime = 0.0;
    std::vector<Move> rootmoves;
    b.legalMoves(rootmoves);
    if (rootmoves.empty()) return best;
    best.m = rootmoves[0];

    for (int depth = 6; depth <= 25; ++depth) {
        auto now = Clock::now();
        if (now >= softEnd) break;

        //dont start a thing u cant finish
        if (depth > 1 && lastDepthTime > 0.0) {
            double estimate = lastDepthTime * 4.0;
            if (now + sec(estimate) > softEnd) break;
        }

        auto depthStart = Clock::now();
        Line candidate = best_move(depth, maximising, b);
        auto depthEnd  = Clock::now();
        lastDepthTime = sec(depthEnd - depthStart).count();

        // only accept the result if we finished before the hard deadline
        if (depthEnd < hardEnd) {
            best = candidate;
        } else {
            break;//overshoot
        }
    }

    return best;
}