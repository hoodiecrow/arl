#include "map9.h"

PART A, B, C, D, E, F, G, H, I;

int** genMap9(int ysize, int xsize) {
    // A B C
    // D E F
    // G H I
    A.ybase = 0;
    A.xbase = 0;
    A.ysize = ysize / 3;
    A.xsize = xsize / 3;
    B.ybase = 0;
    B.xbase = A.xsize;
    B.ysize = A.ysize;
    B.xsize = (xsize - B.xbase) / 2 + B.xbase;
    C.ybase = 0;
    C.xbase = B.xsize;
    C.ysize = A.ysize;
    C.xsize = xsize;
    D.ybase = A.ysize;
    D.xbase = 0;
    D.ysize = (ysize - D.ybase) / 2 + D.ybase;
    D.xsize = A.xsize;
    E.ybase = A.ysize;
    E.xbase = D.xsize;
    E.ysize = D.ysize;
    E.xsize = B.xsize;
    F.ybase = A.ysize;
    F.xbase = E.xsize;
    F.ysize = D.ysize;
    F.xsize = C.xsize;
    G.ybase = D.ysize;
    G.xbase = 0;
    G.ysize = ysize;
    G.xsize = A.xsize;
    H.ybase = D.ysize;
    H.xbase = G.xsize;
    H.ysize = G.ysize;
    H.xsize = B.xsize;
    I.ybase = D.ysize;
    I.xbase = H.xsize;
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
    drawCorr(map, A, B);
    drawCorr(map, B, C);
    drawCorr(map, A, D);
    drawCorr(map, D, E);
    drawCorr(map, B, E);
    drawCorr(map, E, F);
    drawCorr(map, C, F);
    drawCorr(map, D, G);
    drawCorr(map, G, H);
    drawCorr(map, E, H);
    drawCorr(map, H, I);
    drawCorr(map, F, I);
}

void drawCorr(WINDOW* map, PART from, PART to) {
    int frommidpointY = (from.ysize - from.ybase) / 2 + from.ybase;
    int frommidpointX = (from.xsize - from.xbase) / 2 + from.xbase;
    int tomidpointY = (to.ysize - to.ybase) / 2 + to.ybase;
    int tomidpointX = (to.xsize - to.xbase) / 2 + to.xbase;
    mvprintw(1, 0, "%d %d %d %d", frommidpointY, frommidpointX, tomidpointY, tomidpointX); clrtoeol(); refresh();
    if (frommidpointY < tomidpointY) {
        for (int y = frommidpointY; y < tomidpointY; y++) {
            int x = frommidpointX;
            mvwaddch(map, y, x, ' ');
        }
    } else if (frommidpointY > tomidpointY) {
        for (int y = frommidpointY; y > tomidpointY; y--) {
            int x = frommidpointX;
            mvwaddch(map, y, x, ' ');
        }
    } else if (frommidpointX < tomidpointX) {
    //mvprintw(1, 0, "%d %d %d %d", frommidpointX, 0, 0, 0); clrtoeol(); refresh();
        for (int x = frommidpointX; x < tomidpointX; x++) {
            int y = frommidpointY;
            mvwaddch(map, y, x, ' ');
        }
    } else if (frommidpointX > tomidpointX) {
        for (int x = frommidpointX; x > tomidpointX; x--) {
            int y = frommidpointY;
            mvwaddch(map, y, x, ' ');
        }
    }
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
