#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "SDL.h"

#if defined(LINUX) || defined(SOLARIS_X86)
#include <unistd.h>
#if defined(LINUX) && !defined(SOLARIS_X86)
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#define    INADDR_NONE             ((in_addr_t) 0xffffffff)
#endif
#include <netinet/in.h>
#endif                            /* !WIN32 */

#include "highsocket.h"
#include "mytypes.h"
#include "squadre.h"
#include "network.h"
#include "mydebug.h"
#include "os_defs.h"
#include "mymacros.h"

#include "menu_externs.h"

extern struct team_disk *teamlist;
extern struct team_disk leftteam_dk,rightteam_dk;

player *network_player=NULL;
char chat_message[CHATMSGSIZE];
char outgoing_chat[CHATMSGSIZE];

void SendChatMessage(void)
{
    char buffer[sizeof(outgoing_chat)+sizeof(simplemsg)+sizeof(network_player->playername)+2];
    simplemsg *m=(simplemsg *)buffer;
    short l;

    if(!*outgoing_chat)
        return;

    memcpy(m->hdr,SERVER_HEADER,4);
    m->type=MSG_CHAT;
    sprintf(buffer+sizeof(simplemsg),"%s: %s",network_player->playername,outgoing_chat);
    l=strlen(buffer+sizeof(simplemsg));
    m->size=htons(l);

    SockWrite(network_player->socket,buffer,sizeof(simplemsg)+l);
    *outgoing_chat=0;
}
int FindPacket(char *buf,int len)
{
    int i;

//    D(bug( "Received %ld length packet \n",len));

    for(i=0; i<len;i++)
        if(!strncmp(buf+i,SERVER_HEADER,4))
            return i;

    D(bug("Header non trovato nel pacchetto di lunghezza %d\n",len));

    return -1;
}


simplemsg *getmsg(char *buf, int len)
{
    int i=FindPacket(buf,len);

    if(i>=0) {
        unsigned short pl=ntohs(((simplemsg *)(&buf[i]))->size);

        if( (pl+sizeof(simplemsg)+i)<=len) { 
            ((simplemsg *)&buf[i])->size=pl;
            return (simplemsg *)&buf[i];
        }
    }

    return NULL;
}

simplemsg *IsPacket(char *buf, int len, char type)
{
    int i=FindPacket(buf,len);

    if(i>=0) {
        simplemsg *t=(simplemsg *)&buf[i];

        if (t->type ==  type)
            return t;

        D(bug("Ricevuto pacchetto %c aspettando %c....\n",t->type,type));
    }

    return NULL;
}

simplemsg *GetNextMsg(player *p)
{
    int bytes_read=0;
    simplemsg *m;
    
    do {
        if(bytes_read>=SOCKETBUFFERSIZE) {
            D(bug("Impossibile trovare un messaggio intero!\n"));
            return NULL;
        }
        bytes_read+=SockRead(p->socket,p->packetbuffer+bytes_read,SOCKETBUFFERSIZE-bytes_read);
        m=getmsg(p->packetbuffer,bytes_read);
    }
    while(!m);

    D(bug("Messaggio ricevuto.\n"));
    return m;
}

void UpdateNetStatus(char *string)
{
    extern struct myfont *bigfont;
    extern int WINDOW_WIDTH, WINDOW_HEIGHT;

    MyRestoreBack();
    PrintShadow(FixedScaledX(1),FixedScaledY(210),string,strlen(string),bigfont);
    ScreenSwap();
}

int ReceiveTeam(struct team_disk *s)
{
    char buf[1024];
    simplemsg *m;

    D(bug("Ricevo la squadra...\n"));

    UpdateNetStatus("RECEIVING OPPONENT TEAM...");

    if ((m=GetNextMsg(network_player))) {
         if (m->type==MSG_TEAM) {
            FILE *f;

            snprintf(buf, 1024, "%stempremteam", TEMP_DIR);
            if ((f=fopen(buf/*-*/,"wb+"))) {
                fwrite(((char *)m)+sizeof(simplemsg),1,m->size,f);
                fseek(f,0,SEEK_SET);
                ReadTeam(f,s);
                fclose(f);
                D(bug("Ricevuta squadra di %d bytes\n", m->size));
                return TRUE;
            }
        }
        D(else bug("Ricevuto %c aspettando %c\n",m->type,MSG_TEAM));
    }

    SockClose(network_player->socket);
    free(network_player);

    request("ERROR RECEIVING TEAM");

    return FALSE;
}

int SendTeam(int8_t team)
{
    char buf[1024];
    simplemsg *m=(simplemsg *)network_player->packetbuffer;
    short l;
    FILE *f;

    snprintf(buf, 1024, "%stempteam", TEMP_DIR);
    if((f=fopen(buf/*-*/,"wb+"))) {
        WriteTeam(f,&teamlist[team]);
        fseek(f,0,SEEK_SET);
        l=fread(network_player->packetbuffer+sizeof(simplemsg),1,SOCKETBUFFERSIZE,f);
        fclose(f);
        D(bug("Sending team (%d bytes)...\n",l));
        m->size=htons(l);
        m->type=MSG_TEAM;
        
        UpdateNetStatus("SENDING LOCAL TEAM...");
        
        SockWrite(network_player->socket,m,l+sizeof(simplemsg));
        
        os_delay(30);
        
        if((l=SockRead(network_player->socket,network_player->packetbuffer,sizeof(simplemsg)))==sizeof(simplemsg)) {
            if(IsPacket(network_player->packetbuffer,l,MSG_OK)) {
                D(bug("Team inviata correttamente.\n"));
                return TRUE;
            }
        }
    }
    SockClose(network_player->socket);
    free(network_player);

    request("ERROR SENDING TEAM");

    return FALSE;
}

BOOL ConfigureSession(player *p)
{
    char buf[1024];
    simplemsg *m;

    D(bug("Aspetto il ping...\n"));

    if(!(m=GetNextMsg(p)))
        return FALSE;

    if(m->type==MSG_PING) {
        short l=m->size;
        D(bug("Ping ricevuto...\n"));
        m->type=MSG_PONG;
        m->size=htons(l);
        SockWrite(p->socket,m,l+sizeof(simplemsg));
    }
    else
        return FALSE;

    UpdateNetStatus("WAITING FOR CONFIGURATION...");

    D(bug("Aspetto la configurazione...\n"));

    if(!(m=GetNextMsg(p)))
        return FALSE;

    if(m->type==MSG_CONFIG) {
        FILE *f;
        simplemsg msg;

        p->num=m->subtype;

        D(bug("Config ricevuta...il giocatore e' il plr %d\n",p->num));

        memcpy(msg.hdr,SERVER_HEADER,4);
        msg.subtype=0; // da cambiare x vedere il colore voluto.
        msg.size=0;

        snprintf(buf, 1024, "%stempconfig", TEMP_DIR);
        if ((f=fopen(buf/*-*/,"wb+"/*-*/))) {
            extern void load_config(FILE *);

            fwrite(((char *)m) + sizeof(simplemsg), 1, m->size, f);

            D(bug("Scrivo %d bytes in tempconfig...\n", m->size));
            
            msg.type=MSG_OK;
            fseek(f,0,SEEK_SET);
            D(bug("Carico la configurazione...\n"));
            load_config(f); // la close viene fatta dentro
//            fclose(f);
        }
        else
            msg.type=MSG_KO;

        SockWrite(p->socket,&msg,sizeof(simplemsg));
    }
    return TRUE;
}

player *connect_server(char *ip,int team)
{
    player *p;
    char buffer[100];
    struct sockaddr_in sin;
    long addr,l;

    UpdateNetStatus("CONNECTING");

    D(bug("Preparo la connessione di rete...\n"));

    if(!SocketSystemBoot())
        return NULL;
    
    D(bug("Socket aperto correttamente.\n"));

    memset(&sin,0,sizeof(sin));

    addr=inet_addr(ip);

    if(addr==INADDR_NONE)  {
        request("Invalid server address");
        return NULL;
    }

    sin.sin_addr.s_addr=addr;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(4000);

    if ((p=calloc(sizeof(player),1))) {
        if((p->socket=socket(AF_INET,SOCK_STREAM,0))<0) {
            free(p);
            /* AC: Pu˜ fallire anche qui. */
            request("Unable to connect to server.");
            return NULL;
        }

        if( connect(p->socket, (struct sockaddr *) &sin, sizeof(sin) ) < 0 ) {
            free(p);
            request("Unable to connect to server.");
            return NULL;
        }

        D(bug("Connessione eseguita\n" ));

        UpdateNetStatus("CONNECTION ESTABLISHED");

        os_delay(10);

        D(bug("Leggo pacchetto di benvenuto...\n"));

        if((l=SockRead(p->socket,buffer,sizeof(simplemsg)))<sizeof(simplemsg)) {
            SockClose(p->socket);
            free(p);
            request("Data read failed.");
            return NULL;
        }
        else if(IsPacket(buffer,l,MSG_WELCOME)) {
            simplemsg m={SERVER_HDR,MSG_NAME,0,0};

            m.size=htons((unsigned short)strlen(teamlist[team].name));
            memcpy(buffer,&m,sizeof(simplemsg));
            strcpy(buffer+sizeof(simplemsg),teamlist[team].name);
            SockWrite(p->socket,buffer,sizeof(simplemsg)+strlen(teamlist[team].name));

            os_delay(10);
            UpdateNetStatus("FIRST NEGOTIATION");
            os_delay(10);

            D(bug("Aspetto l'ok per la ricezione del nome...\n"));

            if((l=SockRead(p->socket,buffer,sizeof(simplemsg)))<sizeof(simplemsg)) {
                SockClose(p->socket);
                free(p);
                request("Data read failed.");
            }
            else if(IsPacket(buffer,l,MSG_OK)) {
                char buffer[120];

                D(bug("NET: connessione stabilita!\n"));
                UpdateNetStatus("WAITING FOR OPPONENT");

                if(!ConfigureSession(p))
                    goto failedparams;

                sprintf(buffer,"READY TO PLAY AGAINST %s\n","UNKNOWN");
                request(buffer);
            }
            else goto failedparams;
        }
        else {
failedparams:
            request("Failed parameters negotiation...\n");
            SockClose(p->socket);
            free(p);
            return NULL;
        }
    }

    return p;
}

void free_network(void)
{
    if(network_player) {
        SockClose(network_player->socket);
        free(network_player);
    }

    network_player=NULL;
}

typedef struct
{
    simplemsg hdr;
    unsigned long joypos;
}
statusmsg;

typedef struct
{
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

statusmsg mystatusmsg={{SERVER_HDR,MSG_EVENT,0,0},0};

unsigned long NetJoyPos[2]={0,0};

void stripmsg(simplemsg *m,int *len)
{
    int offset=sizeof(simplemsg)+m->size+(int)(((char *)m)-network_player->packetbuffer);

    *len-=offset;

    if(*len>0)
        memcpy(network_player->packetbuffer, network_player->packetbuffer+offset,*len);

//    D(bug("Tolgo %d bytes dal totale ricevuto\n",offset));
}

struct NetData
{
    uint32_t Control0, Control1;
};

struct NetData NetDatas[256];

void SendFinish(unsigned short result)
{
    statusmsg msg;

    memcpy(msg.hdr.hdr, SERVER_HEADER, 4);
    msg.hdr.type = MSG_QUIT;
    msg.hdr.size = htons(sizeof(uint32_t));
    msg.hdr.subtype = SUB_ENDGAME;
    msg.joypos = htonl((result&0xff)|((result&0xff00)<<8));

    SockWrite(network_player->socket, &msg, sizeof(msg));
}

void SendQuit(void)
{
    simplemsg msg;

    memcpy(msg.hdr, SERVER_HEADER, 4);
    msg.type = MSG_QUIT;
    msg.subtype = SUB_LOCAL_QUIT;
    msg.size = 0;

    SockWrite(network_player->socket, &msg, sizeof(msg));
}

static unsigned long network_frame = 0, actual_frame = 0, frames = 0;

void HandleNetwork(unsigned char counter, signed short pallax)
{
    int l; // ,ok=1; not used
    replystatusmsg *msg;
    
// passo di qui solo una volta ogni due frame
    frames++;

    if((frames%2)!=0)
        return;

/* ogni 16 frame mando il mio stato.    
 * lo stato contiene il contatore del generatore di numeri casuali
 * e la posizione X della palla, utilizzando questo numero si controlla
 * che le due partite non abbiano perso coerenza, in tal caso viene
 * inviato un (grosso) messaggio di sync tra le due partite basato 
 * su qualcosa di simile alla gestione del replay.
 */

    if((frames%16) == 0) {
        syncmsg sync={{SERVER_HDR,MSG_SYNC,0,0}, 0, 0};
        
        sync.frame = htonl(frames);
        sync.counter = htonl(counter | (pallax<<8));
        sync.hdr.size = htons(sizeof(syncmsg)-sizeof(simplemsg));
        SockWrite(network_player->socket, &sync, sizeof(sync));
    }

    
tryagain:
    
    while ((l=SockRead(network_player->socket,
                    network_player->packetbuffer+network_player->packetoffset,
                    SOCKETBUFFERSIZE-network_player->packetoffset))>0) {
        fputc('.', stderr);

        l+=network_player->packetoffset;

        while(l>0 && (msg=(replystatusmsg *)getmsg(network_player->packetbuffer,l))) {
            if(msg->hdr.type == MSG_EVENT) {
                NetDatas[network_frame % 256].Control0=ntohl(msg->joypos[0]);
                NetDatas[network_frame % 256].Control1=ntohl(msg->joypos[1]);
                network_frame++;
            }
            else if (msg->hdr.type == MSG_QUIT) {
                extern int quit_game;

                final=FALSE;
                quit_game=TRUE;

                D(bug("Ricevuto quit da remoto.\n"));
            }
            D(else bug("Pacchetto non atteso %c...\n",msg->hdr.type));

            stripmsg( (simplemsg *)msg,&l);
        }
        network_player->packetoffset=l;
    }

    if(network_frame>actual_frame) {
        NetJoyPos[network_player->num]=NetDatas[actual_frame % 256].Control0;
        NetJoyPos[network_player->num^1]=NetDatas[actual_frame % 256].Control1;
        actual_frame++;
    }
    else {
        D(bug("Attenzione nf:%d af:%d\n", network_frame, actual_frame));
        SDL_Delay(1);
        goto tryagain;
    }
        
    fputc(',', stderr);
}

unsigned long LastSentJoyPos=0;

void SendNetwork(unsigned long joypos)
{
    if(joypos!=LastSentJoyPos) {
        mystatusmsg.joypos=htonl(joypos);
//        mystatusmsg.hdr.subtype=p->TabCounter;
        SockWrite(network_player->socket,&mystatusmsg,sizeof(mystatusmsg));
        LastSentJoyPos=joypos;
    }
}


int send_netstart(void)
{
    simplemsg netstart={SERVER_HDR,MSG_READY,0,0};
    char buffer[50];
    int l;

// okkio, non posso mai settare in modo statico le dimensioni delle estensioni!

    mystatusmsg.hdr.size=htons(sizeof(statusmsg)-sizeof(simplemsg));
    actual_frame=network_frame=0;

    D(bug("NET: Client pronto... avviso il server.\n"));
    SockWrite(network_player->socket,&netstart,sizeof(netstart));

    D(bug("NET: Aspetto l'OK\n"));

    if((l=SockRead(network_player->socket,buffer,sizeof(buffer)))>=sizeof(simplemsg)) {
         if(IsPacket(buffer,l,MSG_OK)) {
             D(bug("NET: Ricevuto OK, metto il socket in modalita' non bloccante\n"));
             SockNonBlock(network_player->socket);

             network_player->packetoffset = 0;
            return 1;
        }
         D(bug("NET: Non ho ricevuto il pacchetto di OK...\n"));
    }
    else {
        D(bug("Ricevuto pacchetto troppo corto o errore di rete...\n"));
    }

    return 0;
}

