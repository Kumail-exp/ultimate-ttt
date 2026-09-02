#include "Board.hpp"
#include "Eval.hpp"
#include <iostream>
using namespace std;
int main(){
    Board b;
    int r,c;
    while(true){
        b.print();
        cout<<"eval:"<<Eval(b)<<endl;
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