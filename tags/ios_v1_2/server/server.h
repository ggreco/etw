#ifndef ETWSERVER_H

#define ETWSERVER_H

#define SERVER_HEADER "EtW1"
#define SERVER_HDR {'E','t','W','1'}

#define SOCKETBUFFERSIZE 4096

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

#pragma pack(push, 2)

typedef struct {
	char hdr[4];
	char type,subtype;
	unsigned short size;	
} simplemsg;

typedef struct {
	simplemsg hdr;
	unsigned long joypos;
}
statusmsg;

typedef struct {
	simplemsg hdr;
	unsigned long joypos[2];
}
replystatusmsg;

typedef struct
{
	simplemsg hdr;
	unsigned long frame;
    char counter;
}
syncmsg;

#pragma pack(pop)

#define MAX_SYNCS 4

typedef struct _player {
	struct _player *next;
	int socket;
	int status;
	long pingtime;
	int playertype;
	int packetoffset;
	struct _match *match;
	unsigned long joypos;
	time_t lastinput;
	char playername[40];
	char hostname[50];
	char packetbuffer[SOCKETBUFFERSIZE];
    int actual_sync;
    unsigned long frames[MAX_SYNCS];
    unsigned long syncs[MAX_SYNCS];
} player;

typedef struct _match {
	struct _match *next;
	player *players[2];
	long pingtime;
}
match;

// mezzora di timeout... sono generoso...

#define INACTIVITY_TIMEOUT 1800

// stati possibili per il giocatore.

enum {PLR_LOGGING,PLR_LOADING,PLR_PLAYING,PLR_WAITING_OPPONENT,PLR_PINGING,PLR_CONFIG,PLR_CONFIGURED,PLR_READY,PLR_TEAM,PLR_QUITTING};


// protos
extern void start_server(int);
extern void my_close_all(void);
extern void close_socket(player *);
#endif
