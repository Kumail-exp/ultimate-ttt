#include "Board.hpp"
#include "Engine.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
using Clock = std::chrono::high_resolution_clock;
using namespace std;

void printBoard(const Board& b) {
    //stole this from zammmu
    cout << "\nCurrent board:\n";
    for (int bigRow = 0; bigRow < 3; ++bigRow) {
        for (int smallRow = 0; smallRow < 3; ++smallRow) {
            for (int bigCol = 0; bigCol < 3; ++bigCol) {
                int big = bigRow * 3 + bigCol;
                for (int smallCol = 0; smallCol < 3; ++smallCol) {
                    int small = smallRow * 3 + smallCol;
                    int val = b.get(big, small);
                    char c = (val == 0 ? '.' : (val == 1 ? 'X' : 'O'));
                    cout << c;
                }
                cout << " | ";
            }
            cout << '\n';
        }
        if (bigRow < 2) cout << "----------------\n";
    }

    cout << "Next board: " << (b.next == 9 ? "FREE" : to_string(b.next)) << '\n';
    cout << "Player to move: " << (b.player == 1 ? "X" : "O") << "\n\n";
}

int main() {
    Board b;
    Board::Undo u;
    
    while (b.winner == 0) {
        printBoard(b);

        vector<Move> moves;
        b.legalMoves(moves);

        if (moves.empty()) {
            cout << "no legal moves left. Draw.\n";
            break;
        }

        if (b.player == 2) { 
            unsigned int r, c;
            Move m;
            bool ok = false;
            while (!ok) {
                cout << "enter smallidx> ";
                if(!(cin >> r) ||r > 8){
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "invalid\n";
                    continue;
                }
                if(b.next == 9){
                    cout << "enter bigidx> ";
                    if (!(cin >> c) || c > 8) {
                        cin.clear();
                        cin.ignore(10000,'\n');
                        cout << "invalid\n";
                        continue;
                    }
                    m = {static_cast<uint8_t>(r), static_cast<uint8_t>(c)};
                } else{
                    m = {static_cast<uint8_t>(r), b.next};
                }

                for(const auto& legal : moves){
                    if (legal.smallidx == m.smallidx && legal.bigidx == m.bigidx) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) cout << "illegal move\n";
            }
            b.make(m, u);
        }else{
            auto start = Clock::now();
            Line l = tbest_move(15.0f, true, b);
            auto end = Clock::now();
            std::chrono::duration<double> elapsed = end - start;
            cout << "time: " <<elapsed.count() <<endl;
            cout << "bot plays " <<(int)l.m.smallidx <<","<< (int)l.m.bigidx<<endl;
            cout << "evaluation: "<< l.eval<<endl;
            cout << "depth: " <<l.depth <<endl;
            b.make(l.m,u);
        }
    }

    printBoard(b);

    if (b.winner == 1) cout << "Winner: X\n";
    else if (b.winner == 2) cout << "Winner: O\n";
    else cout << "Draw\n";

    return 0;
}