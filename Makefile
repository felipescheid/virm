CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=

virm: main.c
	$(CC) $(CFLAGS) -o virm main.c $(LIBS)
