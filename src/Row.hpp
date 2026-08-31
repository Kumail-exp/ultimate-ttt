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
    std::string str(){
        return std::bitset<32>(value).to_string();
    }
};