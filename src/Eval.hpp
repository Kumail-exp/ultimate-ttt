#pragma once
#include "Board.hpp"
Move relToUni(Move m){
    return {(m.bigidx/3)*3+(m.smallidx/3),(m.bigidx%3)*3+(m.smallidx%3)};
}
float weights[9]={1.44, 1,  1.44,
                1,  1.71,   1,
                1.44,   1,  1.44};
float evalCell(int bigidx,Board b){
    int x[3]={0},o[3]={0};
    //row 
    for(int i=0;i<3;i++){
        int oc=0,xc=0;
        for(int j=0;j<3;j++){
            int e=b.get(relToUni({3*i+j,bigidx}));
            oc+=e==2?1:0;
            xc+=e==1?1:0;
        }
        if(oc==0){x[xc]++;}
        if(xc==0){o[oc]++;}

        oc=0,xc=0;
        for(int j=0;j<3;j++){
            int e=b.get(relToUni({3*j+i,bigidx}));
            oc+=e==2?1:0;
            xc+=e==1?1:0;
        }
        if(oc==0){x[xc]++;}
        if(xc==0){o[oc]++;}
    }
    int oc=0,xc=0;
    for(int i=0;i<3;i++){
        int e=b.get(relToUni({4*i,bigidx}));
        oc+=e==2?1:0;
        xc+=e==1?1:0;
    }
    if(oc==0){x[xc]++;}
    if(xc==0){o[oc]++;}

    oc=0,xc=0;
    for(int i=0;i<3;i++){
        int e=b.get(relToUni({2*i+2,bigidx}));
        oc+=e==2?1:0;
        xc+=e==1?1:0;
    }
    if(oc==0){x[xc]++;}
    if(xc==0){o[oc]++;}

    return (7*x[2]+2*x[1])-(7*o[2]+2*o[1]);
}
float Eval(Board b){
    //assuming the board hasnt been won yet
    float sval=0.0;
    for(int i=0;i<9;i++){
        float val=0.0;
        if(b.winners[i]!=0){
            val+=-40*b.winners[i]+60;
        }else{
            val+=evalCell(i,b);
        }
        sval+=val*weights[i];
    }
    return sval;
}