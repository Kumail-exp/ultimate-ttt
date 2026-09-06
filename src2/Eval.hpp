static const int LOCAL_POS[9] = {
    3, 2, 3,
    2, 4, 2,
    3, 2, 3
};

static const int GLOBAL_WEIGHT[9] = {
    3, 2, 3,
    2, 5, 2,
    3, 2, 3
};

// classic 9-bit win masks (one bit per cell)
static const int WIN9[8] = {
    0b000000111, // 0 1 2
    0b000111000, // 3 4 5
    0b111000000, // 6 7 8
    0b001001001, // 0 3 6
    0b010010010, // 1 4 7
    0b100100100, // 2 5 8
    0b100010001, // 0 4 8
    0b001010100  // 2 4 6
};
//extract the funky ahh from the bits
inline void extractBits(uint32_t s, int& xBits, int& oBits) {
    xBits = oBits = 0;
    for (int i = 0; i < 9; ++i) {
        int cell = (s>>(2*i))&3;
        if(cell==1){
            xBits|=(1<<i);
        }else{
            if(cell==2){
                 oBits|=(1<<i);
            }
        }
    }
}

inline int evaluateLocal(uint32_t s, bool meIsX){
    int xBits, oBits;
    extractBits(s, xBits, oBits);
    int myBits  = meIsX ? xBits : oBits;
    int oppBits = meIsX ? oBits : xBits;
    int score = 0;

    //positional ahh
    for (int i=0;i<9;++i) {
        if(myBits &(1<<i)){
            score += LOCAL_POS[i];
        }
        if(oppBits&(1<<i)){
            score -= LOCAL_POS[i];
        }
    }

    // threats
    for (int mask:WIN9){
        //holy didnt knew this function existed thanks GCC
        int my  = __builtin_popcount(myBits  & mask);
        int opp = __builtin_popcount(oppBits & mask);
        int emp = __builtin_popcount((~(myBits | oppBits)) & mask);

        if(my==2 && emp==1){ 
            score += 28;
        }else{ 
            if(my==1 && emp==2){ 
                score += 5;
            }
        }

        if(opp==2 && emp==1){ 
            score -= 28;
        }else{ 
            if(opp==1 && emp==2){
                score -= 5;
            }
        }
    }
    return score;
}

float Eval(const Board& b) {
    //relative to only x perspective unlike zammy gay
    float score = 0.0f;
    for (int g = 0; g < 9; ++g) {
        int w = b.checkSmallWin(g);
        int weight = GLOBAL_WEIGHT[g];
        if(w==1){
            score+=140*weight;
        }else if (w == 2){
            score-=140*weight;
        }else if (!b.isSmallFull(g)){
            //local threats idea was mine tbh i recommended zammu to add it
            score += evaluateLocal(b.small[g], true) * weight;
        }
    }
    return score;
}