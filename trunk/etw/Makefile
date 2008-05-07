# general makefile, tries autodetection if available

all:
	@case "`uname -s 2>/dev/null`" in \
	  "Linux") \
	    $(MAKE) -f Makefile.linux $@ \
	    ;; \
	  "Darwin") \
	    $(MAKE) -f Makefile.universal $@ \
	    ;; \
	  *) \
	    echo "Error: please use the makefile for your platform:" \
	    exit 1 \
	    ;; \
	esac

clean:
	rm -rf etw etw.exe *.o x86 ppc

.FORCE:
