#include "Board.hpp"
#include "Eval.hpp"
#include "Engine.hpp"
#include <iostream>
using namespace std;
int main(){
    Board b;
    int r,c;
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
            Line l=tbest_move(5,false,b);
            
            b.move(l.move.smallidx,l.move.bigidx);
            cout<<"evaluation:"<<l.eval<<endl;
            cout<<"depth:"<<l.depth<<endl;
        }
    }
    b.print();
    cout<<"winner:"<<(b.winnercheck()==1?"player":"BOT")<<endl;
    return 0;
}