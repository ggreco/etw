# general makefile, tries autodetection if available

SYSTEM=`uname -s 2>/dev/null`

all:
	@case $(SYSTEM) in \
	  "Linux") \
	    $(MAKE) -f Makefile.linux $@ \
	    ;; \
	  "Darwin") \
	    $(MAKE) -j2 -f Makefile.universal \
	    ;; \
	  "MINGW32_NT-6.1") \
	    $(MAKE) -f Makefile.mingw \
	    ;; \
	  *) \
	    echo "Error: please use the makefile for your platform:" \
	    exit 1 \
	    ;; \
	esac

clean:
	rm -rf etw etw.exe *.o x86 ppc

.FORCE:
