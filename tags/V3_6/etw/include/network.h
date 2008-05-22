#ifndef ETW_NETWORK_H
#define ETW_NETWORK_H

#include "mytypes.h"
#include "squadre.h"

#define SERVER_HEADER "EtW1"
#define SERVER_HDR {'E','t','W','1'}

#define SOCKETBUFFERSIZE 4096
#define CHATMSGSIZE 128

#define MSG_NAME 'N'
#define MSG_READY 'R'
#define MSG_EVENT 'E'
#define MSG_OK 'O'
#define MSG_KO 'K'
#define MSG_WELCOME 'H'
#define MSG_QUIT 'Q'
#define MSG_TEAM 'T'
#define MSG_CONFIG 'C'
#define MSG_PING 'P'
#define MSG_PONG 'p'
#define MSG_CHAT 'c'
#define MSG_SYNC 's'

#define SUB_OPPONENT_LOST 'L'
#define SUB_OPPONENT_QUIT 'O'
#define SUB_ENDGAME 'M'
#define SUB_LOCAL_QUIT 'Q'

typedef struct {
    char hdr[4];
    unsigned char type,subtype;
    unsigned short size;
} simplemsg;

typedef struct _player {
    int socket;
    int num;
    int controltype;
    int packetoffset;
    char playername[40];
    char packetbuffer[SOCKETBUFFERSIZE];
} player;

void free_network(void);
player *connect_server(char *,int );
int send_netstart(void);
void HandleNetwork(unsigned char, signed short);
void SendNetwork(unsigned long);
void SendChatMessage(void);
int SendTeam(int8_t);
void SendQuit(void);
void SendFinish(unsigned short);
int ReceiveTeam(struct team_disk *);

extern unsigned long NetJoyPos[2];
extern player *network_player;
extern char chat_message[CHATMSGSIZE];
extern char outgoing_chat[CHATMSGSIZE];

#endif

