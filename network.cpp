#include "network.hpp"
#include "types.hpp"
int state=handshake;
       int sockfd, connfd, len;
struct packet parse_packet(){
    struct packet retval;
    retval.length=readvarint(connfd);
    retval.id=readvarint(connfd);
    switch(state){
        case handshake:
            handshake_pacc hs_p;
            hs_p.protver=readvarint(connfd);
            read(connfd,&hs_p.addr,255);
            read(connfd,&hs_p.port,2);
            hs_p.nextstate=readvarint(connfd);
            retval.hs_p=hs_p;
        break;
    }
    return retval;
}
void process(){
    struct packet p=parse_packet();
    if()
    while(1);

}
void packet_thread(std::string msg){
    struct sockaddr_in servaddr, cli;

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
