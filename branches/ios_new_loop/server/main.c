#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "highsocket.h"
#include "server.h"


int config_length = 0;
char config_buffer[4000];

int main(int argc,char *argv[])
{
	if(argc!=2)
		printf("Usage: %s portnum\n",argv[0]);
	else 
	{
		int port=atoi(argv[1]);
		FILE *f;

		if(!(f=fopen("etw.cfg","rb"))) {
			printf("Unable to load server default config!\n");
			exit(0);
		}

		config_length=fread(config_buffer+sizeof(simplemsg),1,sizeof(config_buffer)-sizeof(simplemsg),f);
		fclose(f);

		printf("Config file read (%d bytes)\n", config_length);
		
		if(port>32767 || port<4000)
			printf("Port number should be between 4000 and 32767\n");
		else
		{
			printf("Winsock initialization...\n");
			SocketSystemBoot();
			atexit(my_close_all);
			start_server(port);
		}
	}
	return 0;
}

