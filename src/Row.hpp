#pragma once
#include <cstdint>
#include <bitset>
class Row{
    public:
    uint32_t value=0;
    int cellAt(int index){
        return (value>>(24-3*index))&7u;
    }
    void change(int index,u_int8_t val){
        // extracting the first three bits of val
        value&=~(7u << (24- (3 * index)));
        value|=val<< (24- (3 * index));

    }
    std::string bin_str(){
        return std::bitset<32>(value).to_string();
    }
    std::string str(){
        std::string val="";
        for(int i=0;i<9;i++){
            if(i%3==0){
                val+=" ";
            }
            int out=cellAt(i);
            val+=out==0?"-":(out==1?"x":(out==2?"o":"#"));
        }
        return val;
    }
};