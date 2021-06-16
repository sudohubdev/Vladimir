#include "network.hpp"
#include "types.hpp"
#define outdatedmsg "\"Server is on 1.16.4 dumbass (Vladimir server)\""
int state=status;
       int sockfd, connfd, len;
struct packet parse_packet(){
    struct packet retval;
    retval.length=readvarint(connfd);
    retval.id=readvarint(connfd);
    varint sz;
    switch(state){
        case status:
            switch(retval.id.data){
                case 0:
                handshake_pacc hs_p;
                hs_p.protver=readvarint(connfd);
                 sz=readvarint(connfd);
                read(connfd,&hs_p.addr,sz.data);
                read(connfd,&hs_p.port,2);
                hs_p.port=__builtin_bswap16(hs_p.port);
                hs_p.nextstate=readvarint(connfd);
                retval.hs_p=hs_p;
                break;
                case 1:
                   read(connfd,&retval.pingval,sizeof(long));
                break;
            }

        break;
        case login:

            switch(retval.id.data){
                case 0:
                    char* playername=readmc_str(connfd);
                    sprintf("POST https://sessionserver.mojang.com/session/minecraft/join")
                    while(1);
                break;

            }
            break;
    }
    return retval;
}unsigned char* der_pubkey;


void write_packet(struct packet in){
    switch(in.id.data){
        case 0:
        getactualvarintsize(&in.id);
        getactualvarintsize(&in.msg.sz);
        in.length.data=0;
        in.length.data=(in.id.actualsize+sizeof(outdatedmsg)+in.msg.sz.actualsize+2);
        writevarint(in.length,connfd);
        writevarint(in.id,connfd);
        varint t;
        t.actualsize=0;
        t.data=sizeof(outdatedmsg);
        writevarint(t,connfd);
        write(connfd,outdatedmsg,sizeof(outdatedmsg));
        break;
        case 1:
        getactualvarintsize(&in.id);
        in.length.data=in.id.actualsize+sizeof(long);
        writevarint(in.length,connfd);
        writevarint(in.id,connfd);
        write(connfd,&in.pingval,sizeof(long));
        std::cout << "responding ping packet"<<std::endl;
        break;

    }
}
void process(){
    state=status;
    struct packet p=parse_packet();
    switch(p.id.data){
        case 0:
            goto login;
        break;
        case 1:
            write_packet(p);

        break;
        default:
            std::cout << "wrong packet id disconnecting"<<std::endl;
            exit(1);
        break;
    }
    login:

    if(p.hs_p.protver.data!=754){
        struct packet kick;
        kick.id.data=0;

        write_packet(kick);

        goto terminate;
    }
    state=p.hs_p.nextstate.data;
    if(state!=2){
        std::cout  << "what the hell, next state="<<state<<std::endl;
        exit(0);
    }
    p=parse_packet();

    terminate:;

}
void packet_thread(std::string msg){

    struct sockaddr_in servaddr, cli;
    //fuck rsa, we're using bungeecord ip-forwarding

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");



    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
    // Accept the data packet from client and verification

    while(1){
        connfd = accept(sockfd, (SA*)&cli,(socklen_t*) &len);
        if(!connfd){
            printf("ERROR");
            exit(1);
        }
        int pid=fork();
        if(pid==0){
            close(sockfd);
            process();
            exit(0);
        }else close(connfd);
    }
}
