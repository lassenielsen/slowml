programs=$(shell ls src)
version = 2023
OS_AUTO = $(shell uname -s)
COMMENT = _

debug:
	echo "Programs: $(programs)"

default:
	echo "Use: make config, make build, make install, make clean, and if you don't like it make uninstall."

phony: config build install uninstall clean tags

config: Makefile.in
ifeq ($(OS),linux)
	sed -e "s/#OS$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS),osx)
	sed -e "s/#OS$(COMMENT)MAC//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Linux)
	sed -e "s/#OS$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Darwin)
	sed -e "s/#OS$(COMMENT)MAC//g" Makefile.in > Makefile
else
	echo "Unable to recognize os $(OS_AUTO)."
	echo "If linux is used please run"
	echo "OS=linux make config."
	echo "If OS X is used please run"
	echo "OS=osx make config."
	cp Makefile.in Makefile
endif
endif
endif
endif

build: $(addprefix src/,$(programs))
	chmod a+x $(addprefix src/,$(programs))

install: $(addprefix src/,$(programs))
	cp $(addprefix src/,$(programs)) /usr/local/bin/

uninstall:
	cd /usr/local/bin
	rm -f $(programs)
	cd -

clean:
	touch clean~
	touch objects
	rm *~
	cp Makefile.in Makefile
