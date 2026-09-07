#pragma once
#include "Board.hpp"
#include "Eval.hpp"
#include "Move.hpp"
#include "TT.hpp"
#include <cmath>
#include <vector>
#include <chrono>
inline double Minmax(Board& b, int depth, bool maximising,double alpha, double beta,long& nodes) {
    

    //winner type shift
    if (b.winner == 1) return  10000+depth;
    if (b.winner == 2) return -10000-depth;
    if (b.winner == 3) return  0.0;


    std::vector<Move> moves;
    b.legalMoves(moves);
    if (moves.empty()) return 0.0;

    //searching in tt
    auto it=transpositionTable.find(b.i_hash);
    if(it!=transpositionTable.end()){
        TT& entry = it->second;
        if(entry.depth >= depth){
            //again as a reminder i hate alpha beta
            if(entry.flag == EXACT)
                return entry.eval;

            if(entry.flag == LOWERBOUND)
                alpha = std::max(alpha, entry.eval);

            else if(entry.flag == UPPERBOUND)
                beta = std::min(beta, entry.eval);

            if(alpha >= beta)
                return entry.eval;
        }
    }


    if (depth == 0){ return Eval(b);}

    double originalAlpha = alpha;
    double originalBeta = beta;
    double best = maximising ? -INFINITY : +INFINITY;
    Move bestMove = {9,9};
    nodes+=moves.size();
    Move_ordering(moves);
    int champ=-1;
    if(maximising){
        for(Move x : moves){
            Board::Undo u;
            b.make(x,u);
            double eval = Minmax(b,depth-1,false,alpha,beta,nodes);
            b.unmake(u);
            if(eval > best){
                best =eval;
                bestMove=x;
                champ=x.smallidx;
            }
            if(eval>alpha){
                alpha=eval;
            }
            if(beta <= alpha){
                break;
            }
        }
    } else {
        for (Move x : moves) {
            Board::Undo u;
            b.make(x, u);
            double eval = Minmax(b, depth-1, true, alpha, beta,nodes);
            b.unmake(u);
            if (eval < best) {
                best = eval;
                bestMove = x;
                champ=x.smallidx;
            }
            if(eval<beta){
                beta=eval;
            }
            if(beta <= alpha){
                break;  
            }
        }
    }
    
    Flag flag;
    if(best <= originalAlpha)
        flag = UPPERBOUND;
    else if(best >= originalBeta)
        flag = LOWERBOUND;
    else
        flag = EXACT;

    store(b, depth, best, flag);
    MOVE_IMPORTANCE[champ]+0.1;
    return best;
}

struct Line{
    Move m;
    double eval;
    int depth;
};
inline Line best_move(int depth, bool maximising, Board b,long& nodes) {
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
        double result =Minmax(b,depth-1,!maximising,-INFINITY,INFINITY,nodes);
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
inline Line tbest_move(float time, bool maximising, Board b,long& nodes){
    //highly inspired from zammus design
    using Clock = std::chrono::high_resolution_clock;
    using sec = std::chrono::duration<double>;
    nodes=0;
    auto start= Clock::now();
    auto softEnd= start + sec(time * 0.85);
    auto hardEnd= start + sec(time * 0.96);

    Line best = {{9, 9}, 0.0, 0};
    double lastDepthTime = 0.0;
    std::vector<Move> rootmoves;
    b.legalMoves(rootmoves);
    if (rootmoves.empty()) return best;
    best.m = rootmoves[0];

    for (int depth = 9; depth <= 82; depth++) {
        auto now = Clock::now();
        if (now >= softEnd) break;

        //dont start a thing u cant finish
        if (depth > 1 && lastDepthTime > 0.0) {
            double estimate = lastDepthTime * 4.0;
            if (now + sec(estimate) > softEnd) break;
        }

        auto depthStart = Clock::now();
        Line candidate = best_move(depth, maximising, b,nodes);
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