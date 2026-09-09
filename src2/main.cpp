#include "Board.hpp"
#include "Engine.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <unordered_map>
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
inline std::unordered_map<uint64_t, Move> openingBook;

// ------------------------------------------------------------
// Load Openings.txt into the map
// File format (one line per entry):
//   hash,bigidx,smallidx
// ------------------------------------------------------------
bool loadOpeningBook(const std::string& filename = "Openings.txt")
{
    openingBook.clear();

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open " << filename << "\n";
        return false;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string part;

        // hash
        if (!std::getline(ss, part, ',')) continue;
        uint64_t hash = std::stoull(part);

        // bigidx
        if (!std::getline(ss, part, ',')) continue;
        uint8_t big = static_cast<uint8_t>(std::stoi(part));

        // smallidx
        if (!std::getline(ss, part, ',')) continue;
        uint8_t small = static_cast<uint8_t>(std::stoi(part));

        Move m;
        m.bigidx   = big;
        m.smallidx = small;

        openingBook[hash] = m;
        ++count;
    }

    std::cout << "Loaded " << count << " opening moves from " << filename << "\n";
    return true;
}
bool probeOpeningBook(const Board& b, Move& out)
{
    auto it = openingBook.find(b.i_hash);
    if (it == openingBook.end())
        return false;

    out = it->second;
    return true;
}
void add_to_hist(const Board& b){

    std::ofstream out("hist.txt", std::ios::app);

    if (!out){
        std::cerr << "could not open hist.txt\n";
        return;
    }
    out<<"==========;\n";
    
    for (int i = 0; i < 9; i++) {
        if (i) out << ",";
        out << b.small[i];
    }

    out << ";\n";
    out << static_cast<int>(b.next) << ";\n";
    out << static_cast<int>(b.player) << ";\n";
}   
int main() {
    int movenum=0;
    init();
    initPosScore();
    loadOpeningBook("Openings.txt");
    Board b;
    Board::Undo u;
    
    while (b.winner == 0) {
        movenum++;
        if(movenum<=10){
            add_to_hist(b);
            std::cout << "saved\n";
        }
        printBoard(b);

        vector<Move> moves;
        b.legalMoves(moves);

        if (moves.empty()) {
            cout << "no legal moves left. Draw.\n";
            break;
        }

        if (b.player == 1) { 
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
        } else {
            Move bookMove;
            if (probeOpeningBook(b, bookMove)) {
                cout << "Book move: " << (int)bookMove.smallidx
                    << "," << (int)bookMove.bigidx << endl;
                b.make(bookMove, u);
            } else {
                auto start = Clock::now();
                long nodes;
                Line l = tbest_move(9.0f, false, b, nodes);
                auto end = Clock::now();
                std::chrono::duration<double> elapsed = end - start;
                double n = nodes / 1000000.0;
                double t = elapsed.count();
                cout << "time: " << t << endl;
                cout << "nodes(M): " << n << endl;
                cout << "million nodes per second: " << (n / t) << endl;
                cout << "bot plays " << (int)l.m.smallidx << "," << (int)l.m.bigidx << endl;
                cout << "evaluation: " << l.eval << endl;
                cout << "depth: " << l.depth << endl;
                b.make(l.m, u);
            }
        }
    }

    printBoard(b);

    if (b.winner == 1) cout << "Winner: X\n";
    else if (b.winner == 2) cout << "Winner: O\n";
    else cout << "Draw\n";

    return 0;
}