INCLUDE = -I../sqlite-autoconf-3080200
LIB = -lsqlite3 -lpthread

OBJ = caipiao.o function.o
CC = gcc

CFLAGS = -Wall -g ${INCLUDE} 

all: caipiao

caipiao:${OBJ}
	${CC} -o $@ ${OBJ} ${CFLAGS} ${LIB}

.c.o:
	${CC} -c $< ${CFLAGS} ${INCLUDE}


clean:
	-rm -f *.o
	-rm -f *~
     
.PHONY: clean
