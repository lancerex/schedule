CC=gcc
SHARED_FLAGS= -fPIC -shared
CFLAGS= 
CLIBS= -L./
CINCLUDES= -I../include

SRC_PREFIX=../src
SRC=$(SRC_PREFIX)/schedule.c
OBJ=schedule.o
TAR=libsched.so

$(TAR): $(OBJ)
	$(CC) $^ $(CFLAGS) ${SHARED_FLAGS} -o $@

schedule.o: $(SRC)
	$(CC) -c ${CINCLUDES} $^

demo: ${SRC_PREFIX}/demo.c
	${CC} $^ ${CINCLUDES} ${CLIBS} -lsched -o $@


.PHONY: clean

clean: 
	rm $(OBJ) $(TAR) demo 2>/dev/null
