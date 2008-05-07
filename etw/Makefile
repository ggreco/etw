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

all etw:
	@case "`uname -s 2>/dev/null`" in \
	  "Linux") \
	    $(MAKE) -f Makefile.linux $@ \
	    ;; \
	  "Darwin") \
	    $(MAKE) -f Makefile.linux $@ \
	    ;; \
	  *) \
	    echo "Error: please use the makefile for your platform:" \
	    exit 1 \
	    ;; \
	esac

clean:
	rm -rf etw etw.exe *.o

.FORCE:
