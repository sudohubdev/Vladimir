#ifndef __NETWORKH__
#define __NETWORKH__
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
#include "picojson.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <curl/curl.h>
#define PORT 25565
#define SA struct sockaddr
void packet_thread(std::string);
enum states{
    null=0,status,login
};

inline unsigned __int128 uuidtoint128(char *hex) {
    unsigned __int128 val = 0;
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
typedef struct {
    varint protver;
    char addr[255];
    unsigned short port;
    varint nextstate;

}handshake_pacc;

typedef struct{
    __uint128_t uuid;
    mc_str username;

}loginsucc_pacc;

struct packet{
    varint length,id;
    union{
        handshake_pacc hs_p;
        mc_str msg;
        long pingval;
        loginsucc_pacc ls_p;

    };

};

#endif
