CC=clang
CFLAGS=-g -I. -W -Wall -Wextra -pedantic
DEPS=main.h map9.h ah.h maps.h
OBJ=main.o map9.o ah.o weapons.o armour.o things.o monsters.o scrolls.o rings.o potions.o sticks.o traps.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

arl: $(OBJ)
	$(CC) -o $@ $^ -lncurses $(CFLAGS)

maps: maps.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) arl *~

