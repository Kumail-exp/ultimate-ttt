#include "Board.hpp"
#include <iostream>
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
                    cout << c ;
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
            cout << "No legal moves left. Draw.\n";
            break;
        }        

        int choice = -1;
        while (true) {
            cout << "Enter move index: ";
            cin >> choice;
            if (choice >= 0 && choice < (int)moves.size()) break;
            cout << "Invalid choice, try again.\n";
        }

        Move m = moves[choice];
        b.make(m, u);

        if (b.winner != 0) break;
    }

    printBoard(b);

    if (b.winner == 1) cout << "Winner: X\n";
    else if (b.winner == 2) cout << "Winner: O\n";
    else cout << "Draw\n";

    return 0;
}