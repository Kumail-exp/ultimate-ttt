#include "Row.hpp"
#include <iostream>
using namespace std;
int main(){
    Row r;
    cout<<r.str()<<endl;
    r.change(4,1);
    cout<<r.str()<<endl;
    r.change(5,2);
    cout<<r.str()<<endl;
    r.change(6,3);
    cout<<r.str()<<endl;
    cout<<r.cellAt(4)<<endl;
    cout<<r.cellAt(5)<<endl;
    cout<<r.cellAt(6)<<endl;
    return 0;
}