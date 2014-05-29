FLAGS=-O3
# DEFS= -D USE_OPEN_GL -D MACOS
DEFS=

# LIBS= -framework OpenGL -framework GLUT -lm -L/usr/local/lib
LIBS= -lm -L/usr/local/lib

UNAME= $(shell uname)
ifeq ($(UNAME), Linux)
# LIBS= -lGL -lGLU -lglut
LIBS=
# DEFS= -D USE_OPEN_GL
DEFS=
endif


all:dstar

dstar: Dstar.h Dstar.cpp
	g++ ${FLAGS} Dstar.cpp -o dstar ${LIBS} ${DEFS} -std=c++0x

clean:
	rm -f dstar