CC=gcc
CFLAGS=-DDEBUG -g
INCLUDE=-I. -I/usr/local/include
LDFLAGS=-L/usr/local/lib
LIBS=-lfnv
OBJ=cuckoo.o server.o log_entries.o util.o

all: tega

tega: $(OBJ) 
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o tega $(OBJ) $(LIBS)

cuckoo.o: cuckoo.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o cuckoo.o cuckoo.c 

server.o: server.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o server.o server.c

log_entries.o: log_entries.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o log_entries.o log_entries.c

util.o: util.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o util.o util.c

clean:
	rm -f tega *.o conf.db a.out




