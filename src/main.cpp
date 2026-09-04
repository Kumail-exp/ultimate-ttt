#include "Board.hpp"
#include "Eval2.hpp"
#include "Engine2.hpp"
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
        } else {
            tt.clear();//no risk gng
            Line l=tbest_move(10,false,b);
            cout<<"bot playes"<<l.move.smallidx<<","<<l.move.bigidx<<endl;

            //this has been cooking me for a while no more compromise with the safety 
            if (l.move.smallidx<0 || l.move.bigidx<0){
                auto moves=b.legalMoves();
                if(moves.empty()){
                    cout<<"no legal moves so draw"<<endl;
                    break;
                }
                l.move=moves[0];
                cout<<"fallback move "<<l.move.smallidx<<","<<l.move.bigidx <<endl;
            }
            b.move(l.move.smallidx,l.move.bigidx);
            cout<<"evaluation:"<<l.eval<<endl;
            cout<<"depth:"<<l.depth<<endl;
        }
    }
    b.print();
    int w = b.winnercheck();
    if(w==0)
        //i hate indented codes wither stick to brackets or just become python vro
        cout<<"draw"<<endl;
    else
        cout<<"winner:"<<(w==1?"player":"BOT")<<endl;
    return 0;
}