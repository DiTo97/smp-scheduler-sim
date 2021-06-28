PROJECT :=	simulator
CC = 		gcc
CFLAGS =   -Wall -ggdb3 -pedantic
LDFLAGS =  -pthread

HEADERS := 	$(wildcard *.h)
CFILES := 	$(wildcard *.c)
OBJECTS := 	$(wildcard *.o)


all: 		library functions scheduler $(PROJECT)

library: 	librerie.h
			$(CC) -c $(CFLAGS) $<
			
functions:	funzioni.c funzioni.h 
			$(CC) -c $(CFLAGS) $<
			
scheduler:	scheduler.c scheduler.h
			$(CC) -c $(CFLAGS) $<
			
$(PROJECT): main.c
			$(CC) -c $(CFLAGS) $<
			$(CC) $(CFLAGS) $(LDFLAGS) $(wildcard *.o) -o $(PROJECT)
	
clean:
			@rm $(PROJECT)
			@rm $(OBJECTS)
