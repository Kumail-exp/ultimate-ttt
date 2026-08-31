#pragma once
#include "Row.hpp"
#include <iostream>
struct Move{
    int r,c,cellno;
};
class Board{
    public:
    Row board[9];
    // to play in
    Move pre={-1,-1,-1};
    bool Aturn=true;
    int get(int r,int c){
        return board[r].cellAt(c);
    }
    void set(int r,int c,u_int8_t val){
        board[r].change(c,val);
    }
    void print(){
        std::cout<< "Here is your board:"<<std::endl;
        for(int i=0;i<9;i++){
            if(i%3==0){std::cout<<"\n";}
            std::cout<<board[i].str()<<std::endl;
        }
    }
    void move(int r, int c){
        // later i have to add the move based square move, and improve the coordinate system.
        set(r,c,(Aturn?1:2));
        Aturn=!Aturn;                                                                                         
    }
};