CC=clang
CFLAGS=-g -I. -W -Wall -Wextra -pedantic
DEPS=
OBJ=main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

arl: $(OBJ)
	$(CC) -o $@ $^ -lncurses $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) arl *~

