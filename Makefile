#=====================================================================#
#                              SLOWML                                 #
# Makefile.in                                                         #
# This is the Makefile-template for the SlowML machine learning lib.  #
# If the Makefile has been lost, copy this file to Makefile and type  #
# make config. This will generate a Makefile that is configured for   #
# your system. You can then use                                       #
# make [clean | config | build | install | uninstall].                #
# The standard use is:                                                #
# make config; make build; sudo make install[; sudo make uninstall].  #
# - Lasse Nielsen.                                                    #
#=====================================================================#
name = slowml
version = 2021
libname = lib$(name).so
libname = lib$(name).so
#OS_MAClibname = lib$(name).dylib
libversion = .$(version)
#OS_MAClibversion =
COMMENT = OS_
OS_AUTO = $(shell uname -s)

opt = -O3
compiler = g++
ctags = ctags
args = -std=c++11 -fPIC `sdl-config --cflags` $(opt) -I./include/
#OS_MAClibs = 
libs = -ldpl -lpthread

library_objects = \
  objects/linearregressionmodel.o \
  objects/logisticregressionmodel.o \
  objects/vectormap.o \
  objects/onevsall.o \
  objects/network.o

default:
	@echo "Use make config, make build, sudo make install, make clean and if you don't like it sudo make uninstall."

all: config build install clean

phony: default doc config build install uninstall clean

doc: doc/html

doc/html: include/slowml/*.hpp source/*.cpp
	mkdir -p doc/
	doxygen Doxyfile

config:
ifeq ($(OS),linux)
	sed -e "s/#$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS),osx)
	sed -e "s/#$(COMMENT)MAC//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Linux)
	sed -e "s/#$(COMMENT)LINUX//g" Makefile.in > Makefile
else
ifeq ($(OS_AUTO),Darwin)
	sed -e "s/#$(COMMENT)MAC//g" Makefile.in > Makefile
else
	@echo "Unable to recognize os $(OS_AUTO)."
	@echo "If linux is used please run"
	@echo "OS=linux make config."
	@echo "If OS X is used please run"
	@echo "OS=osx make config."
endif
endif
endif
endif

build: $(libname)$(libversion)

include/$(name)/config.hpp:
	@echo "Creating config header"
	@echo "#ifndef CONFIG_HAPI" > include/$(name)/config.hpp
	@echo "#define CONFIG_HAPI" >> include/$(name)/config.hpp
	@echo "#include <string>" >> include/$(name)/config.hpp
#OS_MAC	@echo "#define OS_X" >> include/$(name)/config.hpp
	@echo "#define OS_LINUX" >> include/$(name)/config.hpp
	@echo "#endif" >> include/$(name)/config.hpp

install: $(libname)$(libversion) include/$(name)/vectormap_bnf.hpp include/$(name)/network_bnf.hpp
	@echo "Copying library"
	cp $(libname)$(libversion) /usr/lib/
	ln -f -s /usr/lib/$(libname)$(libversion) /usr/lib/$(libname)
	@echo "Copying include-files"
	mkdir -p /usr/include/$(name)
	cp include/$(name)/*.hpp /usr/include/$(name)/
	chmod -R a+rx /usr/include/$(name)
	@echo "Reindexing libraries"
	ldconfig -n /usr/lib

uninstall:
	@echo "Removing library"
	rm -f /usr/lib/$(libname)*
	@echo "Removing include-files"
	rm -Rf /usr/include/$(name)
	@echo "Removing opt files"
	rm -Rf /opt/$(name)
	@echo "Reindexing libraries"
	ldconfig -n /usr/lib

clean:
	touch clean~
	touch packages
	touch include/$(name)/config.hpp
	touch include/$(name)/config_bnf.hpp
	touch $(libname)$(libversion)
	rm *~
	rm -Rf packages
	rm -Rf objects
	rm include/$(name)/config.hpp
	rm include/$(name)/*_bnf.hpp
	rm $(libname)$(libversion)
#	rm -Rf doc/html/*
	rm -Rf doc/latex
	cp Makefile.in Makefile

$(libname)$(libversion): $(library_objects)
	$(compiler) -shared -Wl,-soname,$(libname).1 -o $(libname)$(libversion) $(library_objects) $(libs)
#OS_MAC	$(compiler) -dynamiclib -o $(libname) $(library_objects) $(libs)

include/$(name)/%_bnf.hpp: bnf/%.bnf
	xxd -i bnf/$*.bnf > include/$(name)/$*_bnf.hpp

objects/%.o: source/%.cpp include/$(name)/*.hpp  include/$(name)/config.hpp include/$(name)/vectormap_bnf.hpp include/$(name)/network_bnf.hpp
	@mkdir -p objects
	$(compiler) -c source/$*.cpp $(args) $(opt) -o objects/$*.o

tags: include/$(name)/*.hpp source/*.cpp
	$(ctags) -a -o ~/.ctags $(PWD)/include/$(name)/*.hpp $(PWD)/source/*.cpp

