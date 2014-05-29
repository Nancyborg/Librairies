#FLAGS= -O3
# DEFS= -D USE_OPEN_GL -D MACOS
#DEFS=

# LIBS= -framework OpenGL -framework GLUT -lm -L/usr/local/lib
#LIBS= -lm -L/usr/local/lib

#UNAME= $(shell uname)
#ifeq ($(UNAME), Linux)
# LIBS= -lGL -lGLU -lglut
#LIBS=
# DEFS= -D USE_OPEN_GL
#DEFS=
#endif

OPTIMIZATION=3
GPFLAGS=-std=c++11
PROJECT=dstar
include $(GCC4MBED_DIR)/build/gcc4mbed.mk

#all:dstar
#
#dstar: Dstar.h Dstar.cpp
#	g++ ${FLAGS} Dstar.cpp -o dstar ${LIBS} ${DEFS} -std=c++0x
#
#clean:
#	rm -f dstar
