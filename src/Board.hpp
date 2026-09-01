#pragma once
#include "Row.hpp"
#include <iostream>
class Board{
    public:
    Row board[9];
    // to play in
    bool freemove=true;
    bool Aturn=true;
    int nextBig=-1;
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
    bool isValid(int r, int c){
        if(freemove){
            return board[r].cellAt(c)==0;
        }

        if(r/3==nextBig/3 && c/3==nextBig%3){
            return board[r].cellAt(c)==0;
        }
        return false;
    }
    void checkfreemove(){
        freemove=board[nextBig/3].cellAt(3*(nextBig%3))>2;
    }
    void move(int smallidx,int cellno=-1){
        // later i have to add the move based square move, and improve the coordinate system.
        //assuming the player always playes legal move;
        if(cellno==-1){
            cellno=nextBig;
        }

        int br=cellno/3,bc=cellno%3;
        int sr=smallidx/3,sc=smallidx%3;
        set(br*3+sr,bc*3+sc,(Aturn?1:2));
        nextBig=sr*3+sc;
        Aturn=!Aturn;
        checkfreemove();                                                                                         
    }
};