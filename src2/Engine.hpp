#pragma once
#include "Board.hpp"
#include "Eval.hpp"
#include "Move.hpp"
#include "TT.hpp"
#include <cmath>
#include <vector>
#include <chrono>
inline double Minmax(Board& b, int depth, bool maximising,double alpha, double beta,long& nodes,int& killer) {
    nodes++ ;

    //winner type shift
    if (b.winner == 1) return  10000+depth;
    if (b.winner == 2) return -10000-depth;
    if (b.winner == 3) return  0.0;


    
    //searching in tt
    auto it=transpositionTable[b.i_hash&(TABLE_SIZE-1)];
    if(it.key==b.i_hash){
        if(it.depth!=-1){
            if(it.depth >= depth){
                //again as a reminder i hate alpha beta
                if(it.flag == EXACT)
                return it.eval;
                
                if(it.flag == LOWERBOUND)
                alpha = std::max(alpha, it.eval);
                
                else if(it.flag == UPPERBOUND)
                beta = std::min(beta, it.eval);
                
                if(alpha >= beta)
                return it.eval;
            }
        }
    }
    std::vector<Move> moves;
    b.legalMoves(moves);
    if (moves.empty()) return 0.0;
    
    if (depth == 0){ return Eval(b);}

    double originalAlpha = alpha;
    double originalBeta = beta;
    double best = maximising ? -INFINITY : +INFINITY;
    Move bestMove = {9,9};
    Move_ordering(moves,killer);
    int champ=-1;
    bool first=true;
    int movenum=0;
    if(maximising){
        double eval;
        for(Move x : moves){
            Board::Undo u;
            b.make(x,u);
            if (movenum < 3 || depth <= 3){
                if(first){
                    first=false;  
                    eval = Minmax(b,depth-1,false,alpha,beta,nodes,killer);
                }else{ 
                    eval=Minmax(b,depth-1,false,alpha,alpha+1,nodes,killer);
                    if(eval > alpha) eval = Minmax(b, depth-1, false, alpha, beta,nodes,killer);
                }
            }else{
                eval = Minmax(b, depth - 2, false, alpha, alpha + 1, nodes, killer);
                
                if (eval > alpha) {
                    eval = Minmax(b, depth - 1, false,alpha, beta, nodes, killer);
                }
            }
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
            movenum++;
        }
    } else {
        double eval;
        for (Move x : moves) {
            Board::Undo u;
            b.make(x, u);
            if (movenum < 3 || depth <= 3){
                if(first){
                    first=false;
                    eval = Minmax(b, depth-1, true, alpha, beta,nodes,killer);
                }else{ 
                    eval = Minmax(b, depth-1, true, beta-1, beta,nodes,killer);
                    if(eval <beta) eval = Minmax(b, depth-1, true, alpha, beta,nodes,killer);
                }
            }else{
                eval = Minmax(b, depth - 2, true,beta - 1, beta, nodes, killer);

                if (eval < beta) {
                    eval = Minmax(b, depth - 1, true,alpha, beta, nodes, killer);
                }
            }
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
            movenum++;
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
    MOVE_IMPORTANCE[champ]+=(depth/100.0);
    killer=champ;
    return best;
}

struct Line{
    Move m;
    double eval;
    int depth;
};
inline Line best_move(int depth, bool maximising, Board b,long& nodes,int& killer) {
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
        double result =Minmax(b,depth-1,!maximising,-INFINITY,INFINITY,nodes,killer);
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
    int k=1;
    //no risk gng
    for (int depth = 1; depth <= 82; depth++) {
        auto now = Clock::now();
        if (now >= softEnd) break;

        //dont start a thing u cant finish
        if (depth > 1 && lastDepthTime > 0.0) {
            double estimate = lastDepthTime * 4.0;
            if (now + sec(estimate) > softEnd) break;
        }

        auto depthStart = Clock::now();
        Line candidate = best_move(depth, maximising, b,nodes,k);
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