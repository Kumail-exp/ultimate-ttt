#pragma once
#include <cstdint>
#include <vector>

struct Move{
    uint8_t smallidx;
    uint8_t bigidx;
};

class Board {
public:
    //ts bout to be the main board
    uint32_t small[9] = {0};

    //like the equivalent of smallones, u get it wont u 
    uint32_t meta = 0;
    
    //i usemy ultimaste technique of being a chud and stealing this technique from the big projevts so its def fast af

    //0 to 8 is just adress and 9 is freemove, agressive space aving type shi
    uint8_t next = 9;
    //1 is x 2 is o
    uint8_t player = 1;
    //one commewnt a day keeps the bugs away 
    //0-ongoing, 1-x win, 2-o win, 3- draw 
    uint8_t winner = 0;

    //inline makes shits faster
    inline int get(int bigidx, int smallidx) const {
        return (small[bigidx] >> (2 * smallidx)) & 3;
    }

    inline void set(int bigidx, int smallidx, int val) {
        small[bigidx] &= ~(3u << (2 * smallidx));
        small[bigidx] |= (val << (2 * smallidx));
    }

    //Now comes the holy moly speed 
    //making ts constexpr for hyper diaper comppiler optmisations
    static constexpr uint32_t WIN_MASKS[8] = {
        0b00000000000000000000000000111111, // cells 0,1,2
        0b00000000000000000000111111000000, // cells 3,4,5
        0b00000000000000111111000000000000, // cells 6,7,8
        0b00000000000000000001000001000001, // cells 0,3,6
        0b00000000000000000100000100000100, // cells 1,4,7
        0b00000000000000010000010000010000, // cells 2,5,8
        0b00000000000000010000000100000001, // cells 0,4,8
        0b00000000000000000100000100010000  // cells 2,4,6
    };

    //0-ongoing, 1-x win, 2-o win
    int checkSmallWin(int b) const {
        uint32_t s = small[b];
        for (uint32_t mask : WIN_MASKS) {
            uint32_t line = s & mask;
            if (line == (mask & 0b010101010101010101)) return 1;
            if (line == (mask & 0b101010101010101010)) return 2;
        }
        return 0;
    }

    bool isSmallFull(int b) const{
        //obv most of these shorthand tricks are stolen (from humans)
        uint32_t s = small[b];
        return ((s | (s >> 1)) & 0b010101010101010101) == 0b010101010101010101;
    }
    //here comes the harder shit to do 
    struct Undo {
        //also not messing up this time
        uint32_t old_small;
        uint32_t old_meta;
        uint8_t  old_next;
        uint8_t  old_player;
        uint8_t  old_winner;
        uint8_t  big, small;
    };

    void make(Move m, Undo& u) {
        u.big = m.bigidx;
        u.small = m.smallidx;
        u.old_small = small[m.bigidx];
        u.old_meta = meta;
        u.old_next = next;
        u.old_player = player;
        u.old_winner = winner;

        set(m.bigidx, m.smallidx, player);

        //update if the big one got updated
        int w = checkSmallWin(m.bigidx);
        if(w){
            meta&= ~(3u<<(2*m.bigidx));
            meta|=(w<<(2*m.bigidx));
        } else if (isSmallFull(m.bigidx)) {
            //remain empty useless condition
        }

        next = m.smallidx;
        if((meta >> (2 * next) & 3) ||isSmallFull(next)){
            next = 9;
        }
        player = 3 - player;
        //total winner wime
        winner = checkMetaWin();

        //detect global draw if meta is completely full and no one won
        if (winner == 0) {
            bool full = true;
            for (int i = 0; i < 9; ++i) {
                if ((meta >> (2 * i) & 3) == 0 && !isSmallFull(i)) {
                    full = false;
                    break;
                }
            }
            if (full) winner = 3;
        }
    }

    void unmake(const Undo& u){
        small[u.big] = u.old_small;
        meta = u.old_meta;
        next = u.old_next;
        player = u.old_player;
        winner = u.old_winner;
    }

    int checkMetaWin() const{
        for (uint32_t mask : WIN_MASKS) {
            uint32_t line = meta & mask;
            if (line == (mask & 0b010101010101010101)) return 1;
            if (line == (mask & 0b101010101010101010)) return 2;
        }
        return 0;
    }
    void legalMoves(std::vector<Move>& moves) const {
        moves.clear();

        if (next == 9){
            for (int b = 0; b < 9; ++b){
                if ((meta >> (2 * b) & 3) == 0 && !isSmallFull(b)){
                    for (int s = 0; s < 9; ++s){ //fancy ahh
                        if (get(b, s) == 0)
                            moves.push_back({(uint8_t)s, (uint8_t)b});
                    }
                }
            }
        } else {
            int b = next;
            //i wont forget that i still have to check wether board is still playable or not
            if ((meta >> (2 * b) & 3) == 0 && !isSmallFull(b)){
                for (int s = 0; s < 9; ++s){
                    if (get(b, s) == 0)
                        moves.push_back({(uint8_t)s, (uint8_t)b});
                }
            }
        }
    }
};