// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
}

typedef struct OBJECT {
    chtype badge;
    int ypos;
    int xpos;
    const char* descr;
    const char* singular;
    int quantity;
    const char* plural;
    bool inInventory;
    struct OBJECT* next;
} OBJECT;

OBJECT* objects = NULL;

OBJECT* newobject(WINDOW* room, chtype badge, int y, int x) {
    OBJECT* object = malloc(sizeof(OBJECT));
    if (object == NULL) {
        mvaddstr(1, 0, "Out of memory, press any key to exit");
        getch();
        exit(1);
    }
    object->badge = badge;
    object->ypos = y;
    object->xpos = x;
    switch (badge) {
        case '$':
            object->descr = "dollar";
            object->singular = "a";
            object->quantity = 12;
            object->plural = "s";
            break;
    }
    object->inInventory = false;
    object->next = objects;
    objects = object;
    mvwaddch(room, y, x, badge);
    wrefresh(room);
    return object;
}

typedef struct SPRITE {
    chtype badge;
    int ypos;
    int xpos;
    const char *descr;
    chtype under;
    struct SPRITE* next;
} SPRITE;

SPRITE* sprites = NULL;

SPRITE* newsprite(WINDOW* room, chtype badge, int y, int x) {
    SPRITE* sprite = malloc(sizeof(SPRITE));
    if (sprite == NULL) {
        mvaddstr(1, 0, "Out of memory, press any key to exit");
        getch();
        exit(1);
    }
    sprite->badge = badge;
    sprite->ypos = y;
    sprite->xpos = x;
    switch (badge) {
        case '@':
            sprite->descr = "the player";
            break;
        case 'a':
            sprite->descr = "carnivorous ape";
            break;
        case 'b':
            sprite->descr = "barbarian";
            break;
    }
    sprite->under = ' ';
    sprite->next = sprites;
    sprites = sprite;
    mvwaddch(room, y, x, badge);
    wrefresh(room);
    return sprite;
}

int sprite_act(WINDOW* room, SPRITE* sprite);
OBJECT* locateObject(int ypos, int xpos);

int main()
{	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    refresh();

    srand (time (NULL));

    int height = 16;
    int width = 20;
    int starty = (LINES - height) / 2;  /* Calculating for a center placement */
    int startx = (COLS - width) / 2;
    WINDOW *room = create_newwin(height, width, starty, startx);
    intrflush(room, FALSE);
    keypad(room, TRUE);

    curs_set(0);
    newobject(room, '$', 2, 4);
    newsprite(room, 'a', 4, 9);
    newsprite(room, 'a', 14, 5);
    newsprite(room, 'b', 12, 3);
    int x = 10;
    int y =  5;
    newsprite(room, '@', y, x);

    int ch;
    SPRITE* sprite;
    do {
        sprite = sprites;
        while (sprite != NULL) {
            ch = sprite_act(room, sprite);
            if (ch == 'Q')
                break;
            sprite = sprite->next;
        }

        refresh();
        wrefresh(room);
    } while (ch != 'Q');

    delwin(room);
	endwin();

	return 0;
}

int sprite_act(WINDOW* room, SPRITE* sprite) {
    int ch;
    chtype floor;
    int y = getmaxy(room);
    int x = getmaxx(room);
    if (sprite->badge == '@') {
        ch = wgetch(room);
    } else {
        ch = 49 + rand() % 8;
    }
    if (ch == '1') {
        //mvprintw(1, 0, "maxy=%d, ypos=%d", y, sprite->ypos);
        if (sprite->xpos > 1 && sprite->ypos < y-2) {
            floor = mvwinch(room, sprite->ypos+1, sprite->xpos-1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, ++sprite->ypos, --sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '2') {
        if (sprite->ypos < y-2) {
            floor = mvwinch(room, sprite->ypos+1, sprite->xpos);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, ++sprite->ypos, sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '3') {
        if (sprite->xpos < x-2 && sprite->ypos < y-2) {
            floor = mvwinch(room, sprite->ypos+1, sprite->xpos+1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, ++sprite->ypos, ++sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '4') {
        if (sprite->xpos > 1) {
            floor = mvwinch(room, sprite->ypos, sprite->xpos-1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, sprite->ypos, --sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '5') {
        mvwaddch(room, sprite->ypos, sprite->xpos, sprite->badge);
    } else if (ch == '6') {
        if (sprite->xpos < x-2) {
            floor = mvwinch(room, sprite->ypos, sprite->xpos+1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, sprite->ypos, ++sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '7') {
        if (sprite->xpos > 1 && sprite->ypos > 1) {
            floor = mvwinch(room, sprite->ypos-1, sprite->xpos-1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, --sprite->ypos, --sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '8') {
        if (sprite->ypos > 1) {
            floor = mvwinch(room, sprite->ypos-1, sprite->xpos);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, --sprite->ypos, sprite->xpos, sprite->badge);
            }
        }
    } else if (ch == '9') {
        if (sprite->xpos < x-2 && sprite->ypos > 1) {
            floor = mvwinch(room, sprite->ypos-1, sprite->xpos+1);
            if (floor == '@' || isalpha(floor)) {
                mvaddstr(1, 0, "bash!");
                clrtoeol();
            } else if (floor == ' ' || ispunct(floor)) {
                mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
                sprite->under = floor;
                mvwaddch(room, --sprite->ypos, ++sprite->xpos, sprite->badge);
            }
        }
    }
    if (sprite->badge == '@') {
        if (ch == 'c') {
            mvaddstr(1, 0, "close what?");
            clrtoeol();
        } else if (ch == 'd') {
            mvaddstr(1, 0, "drop what?");
            clrtoeol();
        } else if (ch == 'e') {
            mvaddstr(1, 0, "eat what?");
            clrtoeol();
        } else if (ch == 'E') {
            mvaddstr(1, 0, "equip what?");
            clrtoeol();
        } else if (ch == 'g') {
            mvaddstr(1, 0, "get what?");
            clrtoeol();
        } else if (ch == 'i') {
            mvaddstr(1, 0, "inventory");
            clrtoeol();
        } else if (ch == 'l') {
            mvaddstr(1, 0, "look in which direction?");
            clrtoeol();
        } else if (ch == 'q') {
            mvaddstr(1, 0, "drink what?");
            clrtoeol();
        } else if (ch == 'r') {
            mvaddstr(1, 0, "read what?");
            clrtoeol();
        } else if (ch == 'u') {
            mvaddstr(1, 0, "use what?");
            clrtoeol();
        } else if (ch == 'w') {
            mvaddstr(1, 0, "wear what?");
            clrtoeol();
        } else if (ch == 'z') {
            mvaddstr(1, 0, "zap in which direction?");
            clrtoeol();
        } else if (ch == ',') {
            if (sprite->under == ' ') {
                mvaddstr(1, 0, "there's nothing to pick up");
            } else {
                OBJECT* o = locateObject(sprite->ypos, sprite->xpos);
                if (o == NULL) {
                    mvaddstr(1, 0, "there's nothing to pick up");
                } else {
                    if (o->quantity == 1) {
                        mvprintw(1, 0, "picked up %s %s", o->singular, o->descr);
                    } else {
                        mvprintw(1, 0, "picked up %d %s%s", o->quantity, o->descr, o->plural);
                    }
                    sprite->under = ' ';
                    o->inInventory = true;
                    o->ypos = -1;
                    o->xpos = -1;
                }
            }
            clrtoeol();
        } else if (ch == '<') {
            mvaddstr(1, 0, "you climbed up a stair");
            clrtoeol();
        } else if (ch == '>') {
            mvaddstr(1, 0, "you went down a stair");
            clrtoeol();
        } else if (ch == '@') {
            mvaddstr(1, 0, "character screen");
            clrtoeol();
        } else if (ch == 19) {
            mvaddstr(1, 0, "save game");
            clrtoeol();
        } else if (ch == KEY_F(1)) {
            mvaddstr(1, 0, "help screen");
            clrtoeol();
            // more keys at roguebasin
        } else 
            ;
    }
    return ch;
}

OBJECT* locateObject(int ypos, int xpos) {
    OBJECT* object = objects;
    while (object != NULL) {
        if (ypos == object->ypos && xpos == object->xpos) {
            return object;
        } else {
            object = object->next;
        }
    }
    return NULL;
}

// objects !  " # $ % & ' () * + , - .  / : ; < = > ?  [ \ ] ^ _ { | } ~
/*

 * */
