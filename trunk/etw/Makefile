CC = gcc
DEBUG = 
DEFINES = -DLINUX -DCD_VERSION

OBJECTS = highdirent.o	  cpu.o		   human.o	  myiff.o	specials.o \
	anim.o		  crowd.o	   intro.o	  os_control.o	squadre.o \
	arbitro.o	  data.o	   league.o	  os_init.o	tables.o \
	arcade.o	  dati_vel.o	   lists.o	  os_video.o	tactics.o \
	chunkyblitting.o  display.o	   loops.o	  palla.o	teams.o \
	commento.o	  etw.o		   main.o	  portiere.o	 \
	computer.o	  etw_locale.o	   menu.o	  radar.o	utility.o \
	config.o	  font.o	   menu_config.o  replay.o	velocita_g.o \
	connection.o	  generic_video.o  menu_data.o	  sound.o	wc.o \
	control.o	  gfx.o		   menu_font.o	  special.o

.c.o: etw.h menu.h
	$(CC) $(CFLAGS) $(DEFINES) -c $<


etw: *.c
	@echo "Usa il makefile per la piattaforma apposita!"
#	$(CC) -o ../etw $(OBJECTS) $(LFLAGS)

clean:
	rm -rf *.o
