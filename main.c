// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
#include <ncurses.h>
#include <ctype.h>

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
}

int main()
{	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    refresh();

    int height = 16;
    int width = 20;
    int starty = (LINES - height) / 2;  /* Calculating for a center placement */
    int startx = (COLS - width) / 2;
    WINDOW *room = create_newwin(height, width, starty, startx);
    intrflush(room, FALSE);
    keypad(room, TRUE);

    curs_set(0);
    mvwaddch(room, 4, 9, 'a');
    mvwaddch(room, 2, 4, '%');
    mvwaddch(room, 12, 3, 'b');
    int x = 10;
    int y =  5;
    mvwaddch(room, y, x, '@');
    wrefresh(room);
    int ch;
    chtype floor;
    chtype under = ' '; // what object, if any, is under character's feet
    do {
        ch = wgetch(room);
        if(ch == '1') {
            floor = mvwinch(room, y+1, x-1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, ++y, --x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '2') {
            floor = mvwinch(room, y+1, x);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, ++y, x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '3') {
            floor = mvwinch(room, y+1, x+1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, ++y, ++x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '4') {
            floor = mvwinch(room, y, x-1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, y, --x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '5') {
            mvwaddch(room, y, x, '@');
                move(1, 0);
                clrtoeol();
        } else if(ch == '6') {
            floor = mvwinch(room, y, x+1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, y, ++x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '7') {
            floor = mvwinch(room, y-1, x-1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, --y, --x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '8') {
            floor = mvwinch(room, y-1, x);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, --y, x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == '9') {
            floor = mvwinch(room, y-1, x+1);
            if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, y, x, under);
                under = floor;
                mvwaddch(room, --y, ++x, '@');
                move(1, 0);
                clrtoeol();
            } else if (isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            }
        } else if(ch == 'c') {
            mvaddstr(1, 0, "close what?");
            clrtoeol();
        } else if(ch == 'd') {
            mvaddstr(1, 0, "drop what?");
            clrtoeol();
        } else if(ch == 'e') {
            mvaddstr(1, 0, "eat what?");
            clrtoeol();
        } else if(ch == 'E') {
            mvaddstr(1, 0, "equip what?");
            clrtoeol();
        } else if(ch == 'g') {
            mvaddstr(1, 0, "get what?");
            clrtoeol();
        } else if(ch == 'i') {
            mvaddstr(1, 0, "inventory");
            clrtoeol();
        } else if(ch == 'l') {
            mvaddstr(1, 0, "look in which direction?");
            clrtoeol();
        } else if(ch == 'q') {
            mvaddstr(1, 0, "drink what?");
            clrtoeol();
        } else if(ch == 'r') {
            mvaddstr(1, 0, "read what?");
            clrtoeol();
        } else if(ch == 'u') {
            mvaddstr(1, 0, "use what?");
            clrtoeol();
        } else if(ch == 'w') {
            mvaddstr(1, 0, "wear what?");
            clrtoeol();
        } else if(ch == 'z') {
            mvaddstr(1, 0, "zap in which direction?");
            clrtoeol();
        } else if(ch == ',') {
            mvaddstr(1, 0, "picked up <item>");
            clrtoeol();
        } else if(ch == '<') {
            mvaddstr(1, 0, "you climbed up a stair");
            clrtoeol();
        } else if(ch == '>') {
            mvaddstr(1, 0, "you went down a stair");
            clrtoeol();
        } else if(ch == '@') {
            mvaddstr(1, 0, "character screen");
            clrtoeol();
        } else if(ch == 19) {
            mvaddstr(1, 0, "save game");
            clrtoeol();
        } else if(ch == KEY_F(1)) {
            mvaddstr(1, 0, "help screen");
            clrtoeol();
            // more keys at roguebasin
        } else 
            ;

        refresh();
        wrefresh(room);
    } while (ch != 'Q');

    delwin(room);
	endwin();

	return 0;
}
