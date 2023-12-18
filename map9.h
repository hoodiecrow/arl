#ifndef arl_map9_h
#define arl_map9_h

#include <ncurses.h>
#include "main.h"

typedef struct PART {
    int ybase;
    int xbase;
    int ysize;
    int xsize;
} PART;

int** genMap9(int ysize, int xsize);
void showMap9(WINDOW* map);
void drawBox(WINDOW* map, int ybeg, int xbeg, int ysiz, int xsiz);
void drawCorr(WINDOW* map, PART from, PART to);

#endif
