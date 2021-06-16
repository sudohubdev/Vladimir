#include "types.hpp"


varint readvarint(int fd){
    int i=0;
    int result=0;
    unsigned char inbyte;
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
void writevarint(varint a,int sockfd) {
     int value=a.data;
    do {
        unsigned char temp = (unsigned char)(value & 0b01111111);
        value >>= 7;
        if (value != 0) {
            temp |= 0b10000000;
        }
        write(sockfd,&temp,1);
    } while (value != 0);
}
void getactualvarintsize(varint *v){
    for(int i=31;i>=0;--i){
        if(v->data&1<<i){
            v->actualsize=(_INTSIZE(i)/7);
            break;
        }
    }
}
void writemc_str(mc_str in,int sockfd){
    writevarint(in.sz,sockfd);
    write(sockfd,&in.data,strlen(in.data));
}
char* readmc_str(int sockfd){
    varint c;
    c=readvarint(sockfd);
    char* n=new char[c.data];
    read(sockfd,n,c.data);
    return n;
}
