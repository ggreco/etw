CC = gcc
DEBUG = 
DEFINES = -DLINUX 
NAME = etwd

CFLAGS = -g -O2 -Wall
OBJECTS = highsocket.o server.o utility.o main.o

.c.o: highsocket.h
	$(CC) $(CFLAGS) $(DEFINES) -c $<

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LFLAGS)

clean:
	rm -rf *.o $(NAME)
