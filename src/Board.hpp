#pragma once
#include "Row.hpp"
#include <iostream>
class Board{
    public:
    Row board[9];
    int winners[9]={0}; //for the bigger board;
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
        freemove=get((nextBig/3)*3,(nextBig%3)*3);
    }
    void move(int smallidx,int cellno=-1){
        // later i have to add the move based square move, and improve the coordinate system.
        //assuming the player always playes legal move;
        if(cellno==-1){
            cellno=nextBig;
        }

        int br=cellno/3,bc=cellno%3;
        int sr=smallidx/3,sc=smallidx%3;
        if (!isValid(br*3+sr,bc*3+sc)) {
            std::cerr << "Invalid" << std::endl;
            return;
        }
        set(br*3+sr,bc*3+sc,(Aturn?1:2));
        nextBig=sr*3+sc;
        Aturn=!Aturn;
        updateall();
        checkfreemove();                                                                                         
    }
    bool checkcellwin(int col){
        int r[3]={1,1,1},c[3]={1,1,1},d[2]={1,1};
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                int e=get((col/3)*3 + i, (col%3)*3 + j);
                r[i]*=e;
                c[j]*=e;
                if(i==j){
                    d[0]*=e;
                }
                if(i+j==2){
                    d[1]*=e;
                }
            }
        }
        for(int i=0;i<3;i++){
            if(r[i]==1 || r[i]==8 || c[i]==1 || c[i]==8) return true;
        }
        return (d[0]==1 || d[1]==8 || d[1]==1 || d[0]==8);
    }
    void update(int col){
        if(!checkcellwin(col)) return;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                set((col/3)*3 + i, (col%3)*3 + j, 3);
            }
        }
    }
    void updateall(){
        for(int i=0;i<9;i++){
            update(i);
        }
    }
};