#include "network.hpp"
#include "types.hpp"
#define outdatedmsg "\"Server is on 1.16.4 dumbass (Vladimir server)\""
#define kickmsg "\"Buy a Minecraft account lol(Vladimir server)\""
int state=null;
       int sockfd, connfd, len;
int msg=0;
using namespace boost::uuids;
struct packet parse_packet(){
    struct packet retval;
    retval.length=readvarint(connfd);
    retval.id=readvarint(connfd);
    varint sz;
    switch(state){
        case null:
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

            }

        break;
        case status:

            switch(retval.id.data){
                case 0:
                   read(connfd,&retval.pingval,sizeof(long));
                break;
            };
        case login:

            switch(retval.id.data){
                case 0:
                    char* playername=readmc_str(connfd);
                    retval.msg.sz.data=strlen(playername);
                    memset(retval.msg.data,0,sizeof(retval.msg.sz.data));
                    strcpy(retval.msg.data,playername);
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
        in.length.data=(in.id.actualsize+(msg?sizeof(kickmsg):sizeof(outdatedmsg))+in.msg.sz.actualsize+2);
        writevarint(in.length,connfd);
        writevarint(in.id,connfd);
        varint t;
        t.actualsize=0;
        t.data=(msg?sizeof(kickmsg):sizeof(outdatedmsg));
        writevarint(t,connfd);
        write(connfd,msg?kickmsg:outdatedmsg,(msg?sizeof(kickmsg):sizeof(outdatedmsg)));
        break;
        case 1:
        getactualvarintsize(&in.id);
        in.length.data=in.id.actualsize+sizeof(long);
        writevarint(in.length,connfd);
        writevarint(in.id,connfd);
        write(connfd,&in.pingval,sizeof(long));
        std::cout << "responding ping packet"<<std::endl;
        case 2:
        getactualvarintsize(&in.id);
        getactualvarintsize(&in.ls_p.username.sz);
        in.length.data=(in.id.actualsize+18+in.ls_p.username.sz.actualsize+16);
        writevarint(in.length,connfd);
        writevarint(in.id,connfd);
        write(connfd,&in.ls_p.uuid,16);
        in.ls_p.username.sz.data=16;
        getactualvarintsize(&in.ls_p.username.sz);
        writevarint(in.ls_p.username.sz,connfd);
        write(connfd,in.ls_p.username.data,16);
        break;

    }
}
char response[600]={0};
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    strncpy(response,(char*)buffer,size*nmemb);
   return size * nmemb;
}

void process(){
    CURL* curl;
         curl=curl_easy_init();

    char url[300];
    char username[18]={0};
    CURLcode retc;
    long connectcode;
    state=null;
    struct packet p=parse_packet();
    state=p.hs_p.nextstate.data;

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
            struct packet kick;

    if(p.hs_p.protver.data!=754){
        struct packet kick;
        kick.id.data=0;

        write_packet(kick);

        exit(1);
    }
    if(state!=2){
        std::cout  << "what the hell, next state="<<state<<std::endl;
        exit(0);
    }
    p=parse_packet();
    strcpy(username,p.msg.data);
    sprintf(url,"api.mojang.com/users/profiles/minecraft/%s",username);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_HTTPGET,1);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
    retc=curl_easy_perform(curl);
    curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE ,&connectcode);
    if(connectcode!=200){
        kick.id.data=0;
        msg=1;
        write_packet(kick);
        exit(1);
    }
    picojson::value v;
    std::string json=response;

    picojson::parse(v,json);
    const picojson::value::object& obj = v.get<picojson::object>();
    picojson::value::object::const_iterator i = obj.begin();
    std::string p_uuid=i->second.to_str();
    sprintf(url,"sessionserver.mojang.com/session/minecraft/profile/%s",p_uuid.c_str());
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_perform(curl);

    p.id.data=0x02;
    p.ls_p.uuid=uuidtoint128((char*)p_uuid.c_str());
    strcpy(p.ls_p.username.data,username);
    p.ls_p.username.sz.data=strlen(username);
    getactualvarintsize(&p.ls_p.username.sz);
    write_packet(p);
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
        errno=0;
        bool t=true;
    setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, (const void*)& t,4);

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
