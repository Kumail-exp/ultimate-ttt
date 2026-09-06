#pragma once
#include "Board.hpp"

const float weights[9] = {
    //highkey lookig good indentation
    1.44f, 1.0f,  1.44f,
    1.0f,  1.71f, 1.0f,
    1.44f, 1.0f,  1.44f
};

float Eval(const Board& b) {
    float score = 0.0f;
    for(int i = 0; i < 9; i++){
        int w =b.checkSmallWin(i);
        if(w == 1){
            score +=weights[i];
        }else{
            if(w == 2){
                score -=weights[i];
            }
        }
    }
    return score;
}