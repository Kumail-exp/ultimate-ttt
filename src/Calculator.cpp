#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include "history.hpp"
#include "Engine.hpp"
#include "Board.hpp"
using namespace std;

int main(){
    //inits lawl
    init();
    initPosScore();

    // Read the whole history file
    ifstream file("hist.txt");
    if (!file) {
        // ts wont happen gng
        return 1;
    }
    string data((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
    file.close();

    vector<Board> boards = loadPositions(data);
    if (boards.empty()){
        cerr<<"no positions found in hist.txt\n";
        return 1;
    }

    cout << "loaded " << boards.size() << " positions\n";
    ofstream out("Openings.txt");
    if(!out){
        //ts wont happejn
        return 1;
    }

    long nodes = 0;// whatever u say my pookie engine

    for (size_t i=0;i<boards.size();++i){
        Board& b = boards[i];
        nodes=0;
        Line best = tbest_move(60, b.player == 1, b, nodes);
        cout<<"depth:"<<best.depth<<endl;
        //saving in a shitty format 
        out << b.i_hash << ','<<(int)best.m.bigidx << ','<<(int)best.m.smallidx<<'\n';
        cout <<"Board "<<(i+1) << "/" << boards.size()<<"  hash=" << hex << b.i_hash << dec<<" -> " << (int)best.m.bigidx << "," << (int)best.m.smallidx<<"  (nodes=" << nodes << ")\n";
    }
    out.close();
    cout << "\nSaved "<<boards.size()<<" openings to Openings.txt\n";
    return 0;
}