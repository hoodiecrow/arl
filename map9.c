#include "map9.h"

PART A, B, C, D, E, F, G, H, I;

int** genMap9(int ysize, int xsize) {
    // A B C
    // D E F
    // G H I
    A.ysize = ysize / 3;
    A.xsize = xsize / 3;
    B.ysize = A.ysize;
    B.xsize = (xsize - A.xsize) / 2 + A.xsize;
    C.ysize = A.ysize;
    C.xsize = xsize;
    D.ysize = (ysize - A.ysize) / 2 + A.ysize;
    D.xsize = A.xsize;
    E.ysize = D.ysize;
    E.xsize = B.xsize;
    F.ysize = D.ysize;
    F.xsize = C.xsize;
    G.ysize = ysize;
    G.xsize = A.xsize;
    H.ysize = G.ysize;
    H.xsize = B.xsize;
    I.ysize = G.ysize;
    I.xsize = C.xsize;
}

// maybe unnecessary
void showMap9(WINDOW* map) {
    drawBox(map, 0, 0, A.ysize, A.xsize);
    drawBox(map, 0, A.xsize, B.ysize, B.xsize);
    drawBox(map, 0, B.xsize, C.ysize, C.xsize);
    drawBox(map, A.ysize, 0, D.ysize, D.xsize);
    drawBox(map, A.ysize, D.xsize, E.ysize, E.xsize);
    drawBox(map, A.ysize, E.xsize, F.ysize, F.xsize);
    drawBox(map, D.ysize, 0, G.ysize, G.xsize);
    drawBox(map, D.ysize, G.xsize, H.ysize, H.xsize);
    drawBox(map, D.ysize, H.xsize, I.ysize, I.xsize);
}

void drawBox(WINDOW* map, int ybeg, int xbeg, int ysiz, int xsiz) {
    ybeg++;
    xbeg++;
    int r = 5;
    for (int y = ybeg, x = xbeg; y <= ysiz; y++) {
        mvwaddch(map, y, x, '#');
    }
    for (int y = ybeg, x = xbeg; x <= xsiz; x++) {
        mvwaddch(map, y, x, '#');
    }
    for (int y = ybeg, x = xsiz; y <= ysiz; y++) {
        mvwaddch(map, y, x, '#');
    }
    for (int y = ysiz, x = xbeg; x <= xsiz; x++) {
        mvwaddch(map, y, x, '#');
    }
    for (int x0 = xbeg; x0 < xbeg + rand() % r; x0++) {
        for (int y = ybeg, x = x0; y <= ysiz; y++) {
            mvwaddch(map, y, x, '#');
        }
    }
    for (int y0 = ybeg; y0 < ybeg + rand() % r; y0++) {
        for (int y = y0, x = xbeg; x <= xsiz; x++) {
            mvwaddch(map, y, x, '#');
        }
    }
    int xrand = xsiz - rand() % r;
    //mvprintw(1, 0, "%d %d %d %d", 0, xrand, xsiz, 0); clrtoeol(); refresh();
    for (int x0 = xrand; x0 < xsiz; x0++) {
        for (int y = ybeg, x = x0; y <= ysiz; y++) {
            mvwaddch(map, y, x, '#');
        }
    }
    int yrand = ysiz - rand() % r;
    for (int y0 = yrand; y0 < ysiz; y0++) {
        for (int y = ysiz, x = xbeg; x <= xsiz; x++) {
            mvwaddch(map, y, x, '#');
        }
    }
}
