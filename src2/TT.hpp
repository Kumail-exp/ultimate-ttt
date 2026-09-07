#pragma once
#include "Board.hpp"
#include "Move.hpp"
#include <cstdint>
#include <random>
#include <unordered_map>
//board
uint64_t k_board[9][9][2];
//extra values:
u_int64_t k_player;
u_int64_t k_next[10];

//initing 
void init(){
    std::mt19937_64 rng(142857);//ts number aint random iykyk

    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){ 
            k_board[i][j][0] = rng();
            k_board[i][j][0] = rng();
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



//now comes the tt
struct TT{
    double eval;
    int depth;
};

std::unordered_map<uint64_t, TT> transpositionTable;

inline double lookup(Board& b,int depth){
    auto it=transpositionTable.find(Hash(b));
    if(it!=transpositionTable.end()){
        TT& entry = it->second;
        if (entry.depth <= depth){ //remember the lower depth here is acually higher 
            return entry.eval;
        }
    }
}

void store(Board& b, int depth,double eval){
    transpositionTable[Hash(b)]={eval,depth};
}
