#pragma once
#include "Board.hpp"
#include "Eval.hpp"
#include <cmath>
#include <vector>
#include <chrono>
double minmax(Board& b, int depth, bool maximising) {
    

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
            double eval = minmax(b,depth-1,false);
            b.unmake(u);
            if(eval > best){
                best =eval;
            }
        }
        return best;
    } else {
        double best = +INFINITY;
        for(Move x : moves){
            Board::Undo u;
            b.make(x,u);
            double eval=minmax(b,depth-1,true);
            b.unmake(u);
            if(eval < best){
                best = eval;
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
        double result =minmax(b,depth-1,!maximising);
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
Line tbest_move(float time_ms,bool maximising, Board b){
    double t=0.0;
    using Clock = std::chrono::high_resolution_clock;
    Line l={{static_cast<uint8_t>(9),static_cast<uint8_t>(9)},0,0};
    for(int i=5;i<25;i++){ 
        auto start = Clock::now();
        l=best_move(i,maximising,b);
        auto end = Clock::now();

        std::chrono::duration<double> elapsed = end - start;
        t+=elapsed.count();
        if(time_ms/2<=t){//atleast one loop
            break;
        }
    }
    return l;
}