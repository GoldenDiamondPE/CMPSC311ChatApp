CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk4)
LIBS = $(shell pkg-config --libs gtk4) -lpthread
example-2: cligui.c
	$(CC) $(CFLAGS) -o cligui cligui.c $(LIBS)