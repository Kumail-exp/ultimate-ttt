#include "Board.hpp"
#include "Eval.hpp"
#include "Engine.hpp"
#include <iostream>
using namespace std;
int main(){
    Board b;
    int r,c;
    int depth;
    cout<<"enter depth for bot to calculate at:";
    cin>>depth;
    while(b.winnercheck()==0){
        b.print();
        if(b.Aturn){
            cout<<"to play in "<<b.nextBig<<endl;
            cout<<"enter smallidx>";
            cin>>r;
            if(b.freemove){
                cout<<"enter bigidx>";
                cin>>c;
                b.move(r,c);
            }else{
                b.move(r);
            }
        }else{
            Line l=best_move(depth,false,b);
            b.move(l.move.smallidx,l.move.bigidx);
            cout<<"evaluation:"<<l.eval<<endl;
        }
    }
    return 0;
}