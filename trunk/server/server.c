#if defined(WIN32)

#include <winsock.h>

#elif defined(__SASC)

#include <ios1.h>
#include <ss/socket.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <proto/exec.h>
#include <dos/dos.h>

extern struct Library *SockBase;

#else

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "highsocket.h"
#include "server.h"
#include "utility.h"

#define GetOpponent(x) ( (x->match->players[0] == x) ?  x->match->players[1] : x->match->players[0])


extern char config_buffer[4000]; // ora e' la meta', e non penso verra' mai espansa...
extern int config_length;

player *playerlist=NULL;
match *matchlist=NULL;

int serversocket;
int total_players=0,skip_dns=0;
match *new_match[10];

simplemsg welcomemsg={SERVER_HDR,MSG_WELCOME,0,0};
simplemsg okmsg={SERVER_HDR,MSG_OK,0,0};
simplemsg komsg={SERVER_HDR,MSG_KO,0,0};
simplemsg quitmsg={SERVER_HDR,MSG_QUIT,0,0};
statusmsg pingmsg={{SERVER_HDR,MSG_PING,0,0},0};
replystatusmsg myeventmsg={{SERVER_HDR,MSG_EVENT,0,0},{0,0}};


// nota, p2 viene notificato qui dell'avvenuta creazione del match con un ping


match *create_match(player *p1,player *p2)
{
	match *m=calloc(sizeof(match),1);

	if(m) {
		m->players[0]=p1;
		m->players[1]=p2;
		p1->match=m;
		p2->match=m;
		printf("Passo allo stato PINGING per %s\n",p2->playername);
		p2->status=PLR_PINGING;
		pingmsg.joypos=gettime();
		SockWrite(p2->socket,&pingmsg,sizeof(pingmsg));		
	}
	return m;
}

void process_player(player *p,int length)
{
	simplemsg *msg;
	int oldlength;

	do {
		oldlength=length;
		
		switch(p->status) {
		case PLR_LOGGING:
			if( (msg = getmsg(p,length)) ) {
				if( (msg->type==MSG_NAME && msg->size<sizeof(p->playername)) ) {
					memcpy(p->playername,((char *)msg)+sizeof(simplemsg),msg->size);
					p->playername[msg->size]=0;
					SockWrite(p->socket,&okmsg,sizeof(okmsg));
					printf("changing player name for %s to %s\n",p->hostname,p->playername);

					if(!p->match)
					{
						printf("Passo allo stato WAITING_OPPONENT per %s\n",p->playername);
						p->status=PLR_WAITING_OPPONENT;
					}
					else {
						printf("Passo allo stato PINGING per %s\n",p->playername);
						p->status=PLR_PINGING;
						pingmsg.joypos=gettime();
						SockWrite(p->socket,&pingmsg,sizeof(pingmsg));
					}
				}
				else {
					SockWrite(p->socket,&komsg,sizeof(komsg));
				}
				stripmsg(p,msg,&length);
			}
			break;
		case PLR_QUITTING:
		case PLR_READY:
		case PLR_CONFIGURED:
		case PLR_WAITING_OPPONENT:
			if( (msg=getmsg(p,length)))
				stripmsg(p,msg,&length);
			break;
		case PLR_PINGING:
			if( (msg=getmsg(p,length)) ) {
				 if(msg->type==MSG_PONG) { // gestisco il ping tramite il semplice statusmsg
					simplemsg *configmsg=(simplemsg *)config_buffer;

					p->pingtime=gettime()-((statusmsg *)msg)->joypos;
					printf("Ping delay for %s is %ld msec\n",p->playername,p->pingtime);
					p->match->pingtime=max(p->pingtime,p->match->pingtime);
					p->status=PLR_CONFIG;
					printf("Passo allo stato CONFIG per %s\n",p->playername);
					memcpy(configmsg->hdr,SERVER_HEADER,4);
					configmsg->type=MSG_CONFIG;
					configmsg->size=htons((short)config_length);
					configmsg->subtype= ((p->match->players[0]==p) ? 0 : 1);
					printf("%s e' il giocatore %d\n",p->playername,configmsg->subtype);
					SockWrite(p->socket,config_buffer,sizeof(simplemsg)+config_length);
				}
				else
					printf("Wrong message %c from player %s\n",msg->type,p->playername);
				stripmsg(p,msg,&length);
			}
			break;
		case PLR_TEAM:
			if( (msg=getmsg(p,length)) ) {
				if(msg->type==MSG_TEAM) {
					short l=msg->size;
					player *p2 = GetOpponent(p);

					printf("Received %s's team.\n",p->playername);
					msg->size=htons(l);
					printf("Sending %s's team to %s (%d bytes)...\n",p->playername,p2->playername,l);
					SockWrite(p2->socket,msg,sizeof(simplemsg)+l);
					SockWrite(p->socket,&okmsg,sizeof(okmsg));
					printf("Passo allo stato LOADING per %s\n", p->playername);
					p->status=PLR_LOADING;
					msg->size=l;
				}				
				else {
					printf("Ricevuto da %s %c in fase TEAM\n", p->playername, msg->type);
				}
				stripmsg(p,msg,&length);
			}
			break;
		case PLR_CONFIG:
			if( (msg = getmsg(p, length)) ) {
				if(msg->type == MSG_OK) {
					player *p2 = GetOpponent(p);

					if(p2->status == PLR_CONFIGURED) {
						printf("Passo allo stato TEAM per %s e %s\n",p->playername,p2->playername);
						p2->status = PLR_TEAM;
						p->status  = PLR_TEAM;
					}
					else {
						printf("Passo allo stato CONFIGURED per %s\n",p->playername);
						p->status = PLR_CONFIGURED;
					}
				}
				stripmsg(p, msg, &length);
			}
			break;
		case PLR_LOADING:
			if( (msg = getmsg(p, length)) ) {
				if(msg->type == MSG_READY) {
					player *p2=GetOpponent(p);

					if(p2->status==PLR_READY) {		
						int i=0;

						printf("***Starting match %s vs %s (max ping %ld)\n",p->playername,p2->playername,p->match->pingtime);
						p->status=PLR_PLAYING;
						p2->status=PLR_PLAYING;
						SockWrite(p->socket,&okmsg,sizeof(okmsg));
						SockWrite(p2->socket,&okmsg,sizeof(okmsg));
// aggiungo il match alla lista
						p->match->next=matchlist;
						matchlist=p->match;
						
						while(new_match[i]) i++;

						new_match[i]=p->match;
					}
					else
					{
						printf("Passo allo stato READY per %s\n",p->playername);
						p->status=PLR_READY;
					}
				}
				else if(msg->type==MSG_PING) {
					short l=msg->size;

					msg->size=htons(l);

					msg->type=MSG_PONG;
					SockWrite(p->socket,msg,sizeof(statusmsg));
					msg->size=l;
				}
				else if(msg->type==MSG_PONG) {
					p->pingtime=gettime()-((statusmsg *)msg)->joypos;
					printf("Ping delay for %s is %ld msec\n",p->playername,p->pingtime);
				}
				else {
					SockWrite(p->socket,&komsg,sizeof(komsg));
				}
				stripmsg(p,msg,&length);
			}
			break;
		case PLR_PLAYING:
			if( (msg = getmsg(p,length)) ) {
//				printf("Ricevuto msg i tipo %c, extrasize %d bytes\n",msg->type,msg->size);
                fprintf(stderr, "r");

				if(msg->type==MSG_EVENT) {
					p->joypos=((statusmsg *)msg)->joypos;
				}
				else if(msg->type==MSG_QUIT) {
					player *p2=GetOpponent(p);

					printf("Player %s wants to quit, moving to state QUITTING!\n",p->playername);
								
					p->status=PLR_QUITTING;

					if(msg->subtype!=SUB_ENDGAME) {
						quitmsg.subtype=SUB_OPPONENT_QUIT;
						SockWrite(p2->socket,&quitmsg,sizeof(quitmsg));
						printf("*** Match interrupted by %s quit\n",p->playername);
					}
					else {
						unsigned long i=ntohl(((statusmsg *)msg)->joypos);
						printf("*** Match result (%s's report): %s vs %s = %ld-%ld\n",
														p->playername,
														p->match->players[0]->playername,
														p->match->players[1]->playername,
														(i&0xffff),(i&0xffff0000)>>16);														
					}
				}
				else if(msg->type==MSG_CHAT) {
					player *p2=GetOpponent(p);
					short l=msg->size;

					msg->size=htons(l);

					SockWrite(p2->socket,msg,sizeof(simplemsg)+l);
					SockWrite(p->socket,msg,sizeof(simplemsg)+l);
					msg->size=l;					
				}
				else if(msg->type==MSG_SYNC)  {
                    p->frames[p->actual_sync % MAX_SYNCS] = ((syncmsg *)msg)->frame;
                    p->syncs[p->actual_sync % MAX_SYNCS] = ((syncmsg *)msg)->counter;
                        
                    p->actual_sync++;

                    if(p->actual_sync >= MAX_SYNCS) {
                        player *p2 = GetOpponent(p);
                        int i,j;

                        if(p2->actual_sync >= MAX_SYNCS) {
                            fputc('s', stderr);

                            for(i = 0; i < MAX_SYNCS;i++) {
                                for(j = 0; j < MAX_SYNCS; j++) {
                                    if(p2->frames[i] == p->frames[j]) {
//                                       fputc('S', stderr);
                                        if(p2->syncs[i] != p->syncs[j]) {
                                            int k;

                                            printf("*** ERROR lost sync at %ld\n\nSync dump:\n",
                                                    p->frames[j]);

                                            for(k=0;k< MAX_SYNCS;k++) {
                                                printf(" 1) frame:%ld sync:%ld -- 2) frame:%ld sync:%ld\n", 
                                                        p->frames[k], p->syncs[k],
                                                        p2->frames[k], p2->syncs[k]);
                                            }

                                        }
                                    }
                                }
                            }
                        p->actual_sync = 0;
                        p2->actual_sync = 0;
                        
                        }
                    }
                }
                else {
					printf("Unexpected msg %c of size %d from player %s!!!\n",msg->type,msg->size, p->playername);
				}

				stripmsg(p,msg,&length);
			}
			break;
		}
	}
	while(oldlength>length && length>0);


	p->packetoffset=length;
}

int process_input(player *p,time_t now)
{
	int l;
	int not_done=0;


	do
	{
		l=SockRead(p->socket,p->packetbuffer+p->packetoffset,sizeof(p->packetbuffer)-p->packetoffset);
		
		if (l<=0) {
			if(errno!=EWOULDBLOCK) {
				printf("Error in socket read!\n");
				return -1;
			}
			else
				l=0;
		}
		else if((l+p->packetoffset)==sizeof(p->packetbuffer)) {
			process_player(p,l+p->packetoffset);
			if(p->packetoffset==SOCKETBUFFERSIZE) {
				printf("Heavy error, closing connection to %s!\n",p->playername);
				return -1;
			}

			p->lastinput=now;
			not_done=1;
		}
	}
	while(not_done);

	if(l>0) {
		process_player(p,l+p->packetoffset);
		p->lastinput=now;
	}

	return l;
}

void free_match(match *m, player *p)
{
	match *t;

	printf("Aborted match %s vs %s...\n",m->players[0]->playername,m->players[1]->playername);

// XXX da vedere se devo metterci condizioni particolari nel caso la partita finisca!
	if(m->players[0] && m->players[0] != p) {
		m->players[0]->match=NULL;
        quitmsg.subtype=SUB_OPPONENT_QUIT;
        SockWrite(m->players[0]->socket, &quitmsg, sizeof(quitmsg));
        close_socket(m->players[0]);
    }
    
	if(m->players[1] && m->players[1] != p) {
		m->players[1]->match=NULL;
        quitmsg.subtype=SUB_OPPONENT_QUIT;
        SockWrite(m->players[1]->socket, &quitmsg, sizeof(quitmsg));
        close_socket(m->players[1]);
    }
    
	if(matchlist==m)
		matchlist=m->next;
	else {
		for(t=matchlist; t->next!=m; t=t->next);

		t->next=m->next;
	}

	free(m);
}

void close_socket(player *p)
{
	player *t;

	printf("Losing player %s...\n",p->playername);

	SockClose(p->socket);
	total_players--;
	
	if(p->match) {
		t=GetOpponent(p);

		quitmsg.subtype=SUB_OPPONENT_LOST;
		SockWrite(t->socket,&quitmsg,sizeof(simplemsg));

		free_match(p->match, p);
	}

	if(playerlist==p)
		playerlist=p->next;
	else {
		for(t=playerlist; t->next!=p; t=t->next);

		t->next=p->next;
	}

	free(p);
}

void new_descriptor(int s)
{
	struct sockaddr_in isa;
	struct hostent *from;
	int t;
    socklen_t i;
	unsigned long l;
	player *p,*p2;
	
	i = sizeof(isa);
	
	if ((t = accept(s, (struct sockaddr *)&isa, &i)) < 0){
		return;
	}
	
	SockNonBlock(t);
	
	if(!(p=calloc(sizeof(player),1))) {
		SockClose(t);
		return;
	}

	if (getpeername(t, (struct sockaddr *) &isa, &i) < 0) {
		*p->hostname = '\0';
	} else if ( skip_dns || !(from = gethostbyaddr((char *)&isa.sin_addr,
									sizeof(isa.sin_addr), AF_INET))) {
		l = isa.sin_addr.s_addr;
		sprintf(p->hostname, "%ld.%ld.%ld.%ld",  (l & 0x000000FF),
			(l & 0x0000FF00) >> 8, (l & 0x00FF0000) >> 16,(l & 0xFF000000) >> 24);
	} else {
		strncpy(p->hostname, from->h_name, sizeof(p->hostname)-1);
		p->hostname[sizeof(p->hostname)-1] = '\0';
	}
	
	total_players++;

	printf("New connection from %s...(players %d)\n",p->hostname,total_players);
	sprintf(p->playername,"player %d",total_players);
	p->socket=t;
	p->lastinput=time(NULL);
	p->status=PLR_LOGGING;
	SockWrite(p->socket,&welcomemsg,sizeof(welcomemsg));

	for(p2=playerlist;p2;p2=p2->next)
		if(!p2->match)
			break;

	p->next=playerlist;
	playerlist=p;

	if(p2 && !p2->match) {
		printf("Found opponent for new player, %s!\n",p2->playername);
		create_match(p,p2);
	}
}

#define CLOCKS_PER_40MS (40*MY_CLOCKS_PER_SEC/1000)
clock_t actual_clock=0,expected_clock=0;

void my_close_all(void)
{
	player *next;

	while(playerlist) {
		next=playerlist->next;

		close_socket(playerlist);

		playerlist=next;
	}
	SockClose(serversocket);

	SocketSystemShutDown();
}

void update_match(match *m)
{
	myeventmsg.joypos[0]=m->players[0]->joypos;
	myeventmsg.joypos[1]=m->players[1]->joypos;
	SockWrite(m->players[0]->socket,&myeventmsg,sizeof(myeventmsg));
	SockWrite(m->players[1]->socket,&myeventmsg,sizeof(myeventmsg));
}

void update_matches(void)
{
	match *m,*m_next;

	for(m=matchlist;m;m=m_next) {
		m_next=m->next;
		update_match(m);
	}
}

void server_loop(void)
{
	fd_set input_set, output_set, exc_set;
	struct timeval timeout ={5,0};
	player *point,*next_point;
	int max_socket;
    int reset_timeout = 1;
	long lastdelay=0;
	time_t nowtime;

	{
		int i;

		for(i=0;i<10;i++)
			new_match[i]=NULL;
	}

	printf("Entering main loop\n");

	for(;;) {
// Reset dei descrittori
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		nowtime=time(NULL);

		FD_SET(serversocket, &input_set);
		max_socket=serversocket;

// Configurazione degli input set
		for(point=playerlist; point; point=point->next) {
			FD_SET(point->socket,&input_set);

			if(point->socket>max_socket)
				max_socket=point->socket;
		}
			
		if(!matchlist) {
			timeout.tv_sec=2;
			timeout.tv_usec=0;
			expected_clock=0;
		}
		else {
			int i=0;

            if(reset_timeout) {
    			timeout.tv_sec=0;
	    		timeout.tv_usec=20000-lastdelay; // un po' meno che 1/50esimo...
            }
            
            if (timeout.tv_usec < 0 ||
                    timeout.tv_usec > 20000)
                timeout.tv_usec = 10000;

			while(new_match[i]) {
				long j,totaldelay;

				printf("Match start: %s vs %s\n",new_match[i]->players[0]->playername,
								new_match[i]->players[1]->playername);

				j=0;

				totaldelay=new_match[i]->pingtime;

// questo ciclo crea un po' di ritardo a seconda del tempo di ping dei giocatori.

				while(j<totaldelay) {
				    update_match(new_match[i]);
					j+=20;
				}

				printf("Entering game loop at %ld!\n",gettime());
				expected_clock=gettime() + 20;
				

				new_match[i]=NULL;
				i++;
			}
		}

		if(select(max_socket+1,&input_set,&output_set,&exc_set,&timeout) > 0)
            reset_timeout = 0;
        else
            reset_timeout = 1;
        
		actual_clock=gettime();

		if(matchlist && expected_clock<actual_clock) {
			expected_clock += 20;
			update_matches();
			lastdelay=0;

            fprintf(stderr, ".");
            
//			printf("clock %ld, expected %ld\n",actual_clock,expected_clock);
			while(expected_clock<actual_clock) {
				lastdelay=(actual_clock-expected_clock) * 1000;

				if(lastdelay>20000) {
					printf("Skipped frame\n");
					lastdelay=0;
					update_matches();
                    fprintf(stderr, "o");
					expected_clock += 20;
				}
				else
					break;
			}
		}

// Controllo se ci sono nuove connessioni

// Controllo gli input dai giocatori

		for (point = playerlist; point; point = next_point)
		{
			next_point = point->next;
	
// innanzitutto controllo se c'e' qualche errore... e chiudo eventualmente il socket

			if (FD_ISSET(point->socket, &exc_set))
			{
				FD_CLR((unsigned)point->socket, &input_set);
				FD_CLR((unsigned)point->socket, &output_set);
				close_socket(point);
			}

/* poi controllo se ci sono dati da processare... e li processo subito..
 * nel caso ci sia un errore allora chiudo il socket
 */
			if (FD_ISSET(point->socket, &input_set)) {
				if (process_input(point,nowtime) < 0)
					close_socket(point);
			} else if ( (nowtime-point->lastinput)>
				INACTIVITY_TIMEOUT) {
// nel caso il socket sia inattivo da troppo... lo chiudo
				close_socket(point);
			}
		}
#ifdef __SASC
		if(SetSignal(0,0)&SIGBREAKF_CTRL_C)
			exit(0);
#endif
		if (FD_ISSET(serversocket, &input_set))
			new_descriptor(serversocket);

	}
}

void start_server(int port)
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;

    myeventmsg.hdr.size=htons(sizeof(myeventmsg)-sizeof(simplemsg));
    pingmsg.hdr.size=htons(sizeof(pingmsg)-sizeof(simplemsg));

    printf("Creating listening on port %d\n",port);

    if ( ( serversocket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
		printf("Errore nella apertura del socket\n");
        exit( 1 );
    }
	
    if ( setsockopt( serversocket, SOL_SOCKET, SO_REUSEADDR,
		(char *) &x, sizeof(x) ) < 0 )
    {
		printf("Errore su SO_REUSEADDR\n");
		SockClose(serversocket);
        exit( 1 );
    }
	
    sa              = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port     = htons((unsigned short)port);
	
    if ( bind( serversocket, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
		printf("Errore in bind()\n");
		SockClose(serversocket);
        exit( 1 );
    }
	
    if ( listen( serversocket, 3 ) < 0 )
    {
		printf("Errore in listen()\n");
		SockClose(serversocket);
        exit( 1 );
    }

	server_loop();
}


