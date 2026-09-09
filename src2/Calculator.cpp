#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

#include "history.hpp"   // contains loadPositions / loadPosition
#include "Engine.hpp"
#include "Board.hpp"

using namespace std;

int main()
{
    // Must initialise Zobrist keys + evaluation tables
    init();
    initPosScore();

    // Read the whole history file
    ifstream file("hist.txt");
    if (!file) {
        cerr << "Error: could not open hist.txt\n";
        return 1;
    }

    string data(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );
    file.close();

    // Convert text → vector of Board objects
    vector<Board> boards = loadPositions(data);
    if (boards.empty()) {
        cerr << "No positions found in hist.txt\n";
        return 1;
    }

    cout << "Loaded " << boards.size() << " positions\n";

    ofstream out("Openings.txt");
    if (!out) {
        cerr << "Error: could not create Openings.txt\n";
        return 1;
    }

    long nodes = 0;
    int k = 0;          // whatever your best_move expects

    for (size_t i = 0; i < boards.size(); ++i)
    {
        Board& b = boards[i];          // reference – no unnecessary copy

        nodes = 0;
        k = 0;

        // Adjust the call to match your actual best_move / tbest_move signature
        Line best = tbest_move(25, b.player == 1, b, nodes);

        // Save: hash, bigidx, smallidx
        out << b.i_hash << ','
            << (int)best.m.bigidx << ','
            << (int)best.m.smallidx
            << '\n';

        cout << "Board " << (i + 1) << "/" << boards.size()
             << "  hash=0x" << hex << b.i_hash << dec
             << "  -> " << (int)best.m.bigidx << "," << (int)best.m.smallidx
             << "  (nodes=" << nodes << ")\n";
    }

    out.close();
    cout << "\nSaved " << boards.size() << " openings to Openings.txt\n";
    return 0;
}