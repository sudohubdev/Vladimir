#include "types.hpp"
varint readvarint(int fd){
    int i=0;
    int result=0;
    char inbyte;
    do{
        read(fd,(void*)&inbyte,1);

        int v=(inbyte&0b01111111);
        result|=(v<<(7*i));
        if(++i>5){
            std::cout << "E R R O R" << std::endl;
        }

    } while ((inbyte & 0b10000000) != 0);
    varint ret;
    ret.actualsize=i;
    ret.data=result;
    return ret;
}
