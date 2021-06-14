#ifndef __NETWORK__
#define __NETWORK__
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#define PORT 25565
#define SA struct sockaddr
void packet_thread(std::string);
enum states{
    handshake=0,status,login
};

typedef struct{
    char actualsize;
    int data;
}varint;
typedef struct {
    varint protver;
    char addr[255];
    unsigned short port;
    varint nextstate;

}handshake_pacc;
struct packet{
    varint length,id;
    union{
        handshake_pacc hs_p;
    };

};

#endif
