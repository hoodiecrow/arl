
#ifndef arl_maps_h
#define arl_maps_h

#include "ncurses.h"

int** genMap(int ysize, int xsize, int fill, int r1, int r2, int count);
void showMap(WINDOW* map);

#endif
