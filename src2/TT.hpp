#pragma once
#include "Board.hpp"
#include "Move.hpp"
#include <cstdint>
#include <random>

int constexpr TABLE_SIZE=1048576;
//initing 
void init(){
    std::mt19937_64 rng(142857);//ts number aint random iykyk
    
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){ 
            k_board[i][j][0] = rng();
            k_board[i][j][1] = rng();
        }
    }
    k_player=rng();
    for(int i=0;i<10;i++) k_next[i]=rng();
}
inline uint64_t Hash(Board& b){
    uint64_t hs=0;
    
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){ 
            int v=b.get(i,j);
            if(v==1) hs^=k_board[i][j][0];
            if(v==2) hs^=k_board[i][j][1];
        }
    }
    
    if(b.player==1) hs^=k_player;
    hs^=k_next[b.next];
    return hs;
}


//i tbh dont properly understand alpha-beta pruuuning but whatever it takes to improve it
enum Flag{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

//now comes the tt
struct TT{
    double eval;
    int depth;
    Flag flag;
};
TT transpositionTable[TABLE_SIZE]={{-1,-1,EXACT}};

void store(Board& b, int depth,double eval,Flag f){
    transpositionTable[b.i_hash&(TABLE_SIZE-1)]={eval,depth,f};
}
