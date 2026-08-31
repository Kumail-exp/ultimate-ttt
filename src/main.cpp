#include "Board.hpp"
#include <iostream>
using namespace std;
int main(){
    Board b;
    int r,c;
    while(true){
        b.print();
        cin>>r;
        cin>>c;
        b.move(r,c);
    }
    return 0;
}