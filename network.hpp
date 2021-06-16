#ifndef __NETWORK__
#define __NETWORK__
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "types.hpp"
#include <thread>
#include <unistd.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#define PORT 25565
#define SA struct sockaddr
void packet_thread(std::string);
enum states{
    status=1,login
};

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
        mc_str msg;
        long pingval;

    };

};

#endif
