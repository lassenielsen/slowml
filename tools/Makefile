sources=$(notdir $(shell ls source/*.cpp))
programs = $(sources:.cpp=)
version = 2021
compiler = g++
opt = -O3
args = -std=c++11
libs = -lslowml -ldpl
OS_AUTO = $(shell uname -s)
COMMENT = _

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

build: $(programs)

install: $(programs)
	cp $(programs) slowml_imgvec.py /usr/local/bin/

uninstall:
	cd /usr/local/bin
	rm -f $(programs)
	cd -

clean:
	touch clean~
	touch objects
	touch $(programs)
	rm *~
	rm -Rf objects
	rm $(programs)
	cp Makefile.in Makefile

objects/%.o: source/%.cpp
	mkdir -p objects
	$(compiler) -c source/$*.cpp $(args) $(opt) -o objects/$*.o

%: objects/%.o
	$(compiler) objects/$*.o -o $* $(args) $(opt) $(libs)
