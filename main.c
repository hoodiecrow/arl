// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
#include <ncurses.h>

int main()
{	
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();

    curs_set(0);
    refresh();
    int x = 10;
    int y =  5;
    mvaddch(y, x, '@');
    int ch;
    do {
        ch = getch();
        mvaddch(y, x, ' ');
        if(ch == '1')
            mvaddch(++y, --x, '@');
        else if(ch == '2')
            mvaddch(++y, x, '@');
        else if(ch == '3')
            mvaddch(++y, ++x, '@');
        else if(ch == '4')
            mvaddch(y, --x, '@');
        else if(ch == '5')
            mvaddch(y, x, '@');
        else if(ch == '6')
            mvaddch(y, ++x, '@');
        else if(ch == '7')
            mvaddch(--y, --x, '@');
        else if(ch == '8')
            mvaddch(--y, x, '@');
        else if(ch == '9')
            mvaddch(--y, ++x, '@');
        else if(ch == 'c') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "close what?");
            clrtoeol();
        } else if(ch == 'd') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "drop what?");
            clrtoeol();
        } else if(ch == 'e') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "eat what?");
            clrtoeol();
        } else if(ch == 'E') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "equip what?");
            clrtoeol();
        } else if(ch == 'g') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "get what?");
            clrtoeol();
        } else if(ch == 'i') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "inventory");
            clrtoeol();
        } else if(ch == 'l') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "look in which direction?");
            clrtoeol();
        } else if(ch == 'q') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "drink what?");
            clrtoeol();
        } else if(ch == 'r') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "read what?");
            clrtoeol();
        } else if(ch == 'u') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "use what?");
            clrtoeol();
        } else if(ch == 'w') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "wear what?");
            clrtoeol();
        } else if(ch == 'z') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "zap in which direction?");
            clrtoeol();
        } else if(ch == ',') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "picked up <item>");
            clrtoeol();
        } else if(ch == '<') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "you climbed a stair");
            clrtoeol();
        } else if(ch == '>') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "you went down a stair");
            clrtoeol();
        } else if(ch == '@') {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "character screen");
            clrtoeol();
        } else if(ch == KEY_F(1)) {
            mvaddch(y, x, '@');
            mvaddstr(1, 0, "help screen");
            clrtoeol();
        // more keys at roguebasin
        } else 
            mvaddch(y, x, '@');
        refresh();			/* Print it on to the real screen */
    } while (ch != 'Q');
	endwin();			/* End curses mode		  */

	return 0;
}
