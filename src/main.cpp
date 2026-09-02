#include "Board.hpp"
#include <iostream>
using namespace std;
int main(){
    Board b;
    int r,c;
    while(true){
        b.print();
        for(Move x :b.legalMoves()){

            std::cout<<"("<<x.smallidx<<","<<x.bigidx<<"),";
        }
        std::cout<<"\n";
        cin>>r;
        if(b.freemove){
            cin>>c;
            b.move(r,c);
        }else{
            b.move(r);
        }
    }
    return 0;
}