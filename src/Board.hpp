#pragma once
#include "Row.hpp"
#include <iostream>
#include <vector>
struct Move{
    int smallidx,bigidx;
};
class Board{
    public:
    Row board[9];
    int winners[9]={0}; //for the bigger board;
    bool freemove=true;
    bool Aturn=true;
    int nextBig=-1;
    int get(int r,int c){
        return board[r].cellAt(c);
    }
    int get(Move m){
        //assuming that the m has r,c in pace of smallidx,bigidx
        return board[m.smallidx].cellAt(m.bigidx);
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
    bool isValid(int r, int c) {
        if (winners[(r / 3) * 3 + (c / 3)] != 0) {
            return false;
        }
        if (freemove) {
            return board[r].cellAt(c) == 0;
        }
        if (r / 3 == nextBig / 3 && c / 3 == nextBig % 3) {
            return board[r].cellAt(c) == 0;
        }
        
        return false;
    }

    int pop(int smallidx,int bigidx){
        int r=(bigidx/3)*3+(smallidx/3),c=(bigidx%3)*3+(smallidx%3);
        int val=get(r,c);
        set(r,c,0);
        update(bigidx);
        nextBig=bigidx;
        Aturn=!Aturn;
        checkfreemove();
        return val;
    }
    void checkfreemove(){
        freemove=winners[nextBig]!=0;
        if(freemove){
            return;
        }
        freemove=true;
        for(int i=0;i<3;i++){   
            for(int j=0;j<3;j++){
                if(get((nextBig/3)*3 + i, (nextBig%3)*3 + j)==0){
                    freemove=false;
                    return;
                }
            }
        }
    }
    std::vector<Move> legalMoves(){
        std::vector<Move> moves;
        if(!freemove){
            for(int i=0;i<9;i++){
                if(isValid((nextBig/3)*3+(i/3),(nextBig%3)*3+(i%3))){
                    moves.push_back({i,nextBig});
                }
            }
            return moves;
        }
        for(int r=0;r<9;r++){
            for(int i=0;i<9;i++){
                if(isValid((r/3)*3+(i/3),(r%3)*3+(i%3))){
                        moves.push_back({i,r});
                }
            }
        }
        return moves;
    }
    void move(int smallidx,int cellno=-1){
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
        update(cellno);
        checkfreemove();                                                                                         
    }
    int checkcellwin(int col){
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
            if(r[i]==1 || c[i]==1 ) return 1;
            if(r[i]==8 || c[i]==8 ) return 2;
        }
        if(d[0]==1 || d[1]==1 ) return 1;
        if(d[0]==8 || d[1]==8 ) return 2;
        return 0;
    }
    void update(int col){
        winners[col]=checkcellwin(col);
    }
    void updateall(){
        for(int i=0;i<9;i++){
            update(i);
        }
    }
};