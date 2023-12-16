#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    T_Sprite,
    T_Item,
    T_Structure,
} ThingType;

typedef struct THING {
    ThingType type;
    chtype badge;
    WINDOW* room;
    int ypos;
    int xpos;
    const char* descr;
    bool inInventory;
    bool isEdible;
    bool isPotable;
    bool isEquippable;
    struct THING* next;
    int attack;
    int endurance;
    chtype under;
} THING;

THING* things = NULL;

THING* newThing(WINDOW* win, ThingType type, chtype badge, int y, int x);

#define INVENTORY_SIZE 15
THING* inventory[INVENTORY_SIZE];
int inventoryFill = 0;

THING* worn = NULL;

THING* addArmour(WINDOW* win, int y, int x, const char* descr);
void present(THING* thing);
int sprite_act(WINDOW* room, THING* sprite);
THING* locateThing(int ypos, int xpos);
THING* locateObject(int ypos, int xpos);
void stepSprite(WINDOW* room, THING* thing, chtype floor, int toY, int toX);
void combat(THING* thing, int atY, int atX);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main() {	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    refresh();

    srand (time (NULL));

    int height = 16;
    int width = 20;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    WINDOW *room = create_newwin(height, width, starty, startx);
    keypad(room, TRUE);

    curs_set(0);
    newThing(room, T_Item, '$', 2, 4);
    newThing(room, T_Item, ':', 6, 10);
    newThing(room, T_Item, '*', 12, 9);
    newThing(room, T_Item, '!', 10, 10);
    newThing(room, T_Structure, '>', 4, 12);
    newThing(room, T_Sprite, 'a', 4, 9);
    newThing(room, T_Sprite, 'a', 14, 5);
    newThing(room, T_Sprite, 'b', 12, 3);
    int x = 10;
    int y =  5;
    newThing(room, T_Sprite, '@', y, x);
    addArmour(room, 12, 10, "yellow jammies");
    addArmour(room, 2, 6, "green jammies");

    int ch;
    THING* thing;
    do {
        thing = things;
        while (thing != NULL) {
            if (thing->type == T_Sprite) {
                ch = sprite_act(room, thing);
            }
            if (ch == 'Q')
                break;
            thing = thing->next;
        }

        refresh();
        wrefresh(room);
    } while (ch != 'Q');

    delwin(room);
	endwin();

	return 0;
}

int sprite_act(WINDOW* room, THING* sprite) {
    int ch;
    chtype floor;
    if (sprite->type != T_Sprite)
        return 0;
    if (sprite->endurance <= 0)
        return 0;
    int y = getmaxy(room);
    int x = getmaxx(room);
    if (sprite->badge == '@') {
        ch = wgetch(room);
    } else {
        ch = 49 + rand() % 8;
    }
    int toY;
    int toX;
    if (ch == '1') {
        toY = sprite->ypos+1;
        toX = sprite->xpos-1;
        if (sprite->ypos < y-2 && sprite->xpos > 1) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '2') {
        toY = sprite->ypos+1;
        toX = sprite->xpos;
        if (sprite->ypos < y-2) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '3') {
        toY = sprite->ypos+1;
        toX = sprite->xpos+1;
        if (sprite->ypos < y-2 && sprite->xpos < x-2) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '4') {
        toY = sprite->ypos;
        toX = sprite->xpos-1;
        if (sprite->xpos > 1) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '5') {
        present(sprite);
    } else if (ch == '6') {
        toY = sprite->ypos;
        toX = sprite->xpos+1;
        if (sprite->xpos < x-2) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '7') {
        toY = sprite->ypos-1;
        toX = sprite->xpos-1;
        if (sprite->ypos > 1 && sprite->xpos > 1) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '8') {
        toY = sprite->ypos-1;
        toX = sprite->xpos;
        if (sprite->ypos > 1) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    } else if (ch == '9') {
        toY = sprite->ypos-1;
        toX = sprite->xpos+1;
        if (sprite->ypos > 1 && sprite->xpos < x-2) {
            floor = mvwinch(room, toY, toX);
            if (floor == '@' || isalpha(floor)) {
                combat(sprite, toY, toX);
            } else if (floor == ' ' || ispunct(floor)) {
                stepSprite(room, sprite, floor, toY, toX);
            }
        }
    }
    if (sprite->badge == '@') {
        if (ch == 'c') {
            mvaddstr(1, 0, "close what?");
            clrtoeol();
        } else if (ch == 'e') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to eat:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory && t->isEdible)
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            //TODO allow only selected letters
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                //TODO effect
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
                t->inInventory = false;
            }
        } else if (ch == 'E') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to equip:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory && t->isEquippable)
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            //TODO allow only selected letters
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                //TODO effect
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
                t->inInventory = false;
            }
        } else if (ch == 'g') {
            mvaddstr(1, 0, "get what?");
            clrtoeol();
        } else if (ch == 'i') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "You are carrying:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory)
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
        } else if (ch == 'l') {
            // 1st pass: count the items
            int itemcount = 0;
            for (THING* t = things; t != NULL; t = t->next) {
                if (t->badge != '@' && !t->inInventory && t != worn)
                    itemcount++;
            }
            WINDOW* invlist = create_newwin(itemcount+3, 30, 2, 0);
            // 2nd pass: display the items
            int i = 1;
            mvwprintw(invlist, i++, 2, "%s", "Looking around, you see:");
            for (THING* t = things; t != NULL; t = t->next) {
                if (t->badge != '@' && !t->inInventory && t != worn) {
                    mvwprintw(invlist, i++, 2, "%s", t->descr);
                }
            }
            wrefresh(invlist);
            wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
        } else if (ch == 'q') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to drink:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory && t->isPotable)
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            //TODO allow only selected letters
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                //TODO effect
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
                t->inInventory = false;
            }
        } else if (ch == 'r') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to read:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory &&
                        (t->badge == '~' || t->badge == '#')) {
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
                }
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            //TODO allow only selected letters
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                //TODO effect
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
                t->inInventory = false;
                //TODO item is expended
            }
        } else if (ch == 'u') {
            mvaddstr(1, 0, "use what?");
            clrtoeol();
        } else if (ch == 'w') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to wear:");
            for (int i = 0; i < inventoryFill; i++) {
                THING* t = inventory[i];
                if (t->type == T_Item && t->inInventory && t->badge == '&') {
                    mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
                }
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            //TODO allow only selected letters
            if (i >= 0 && i < inventoryFill) {
                if (worn != NULL) {
                    sprite->under = worn->badge;
                    worn->ypos = sprite->ypos;
                    worn->xpos = sprite->xpos;
                }
                worn = inventory[i];
                inventory[i]->inInventory = false;
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
            }
        } else if (ch == 'z') {
            mvaddstr(1, 0, "zap in which direction?");
            clrtoeol();
        } else if (ch == ',') {
            if (sprite->under == ' ') {
                mvaddstr(1, 0, "there's nothing to pick up");
            } else if (inventoryFill == INVENTORY_SIZE) {
                mvaddstr(1, 0, "your inventory is full");
            } else {
                THING* t = locateObject(sprite->ypos, sprite->xpos);
                if (t == NULL) {
                    mvaddstr(1, 0, "there's nothing to pick up");
                } else if (t->type == T_Structure) {
                    mvaddstr(1, 0, "you can't pick that up");
                } else {
                    inventory[inventoryFill++] = t;
                    mvprintw(1, 0, "%s", t->descr);
                    sprite->under = ' ';
                    t->inInventory = true;
                    t->ypos = -1;
                    t->xpos = -1;
                }
            }
            clrtoeol();
        } else if (ch == '.') {
            WINDOW* invlist = create_newwin(inventoryFill+3, 30, 2, 0);
            mvwprintw(invlist, 1, 2, "%s", "What do you want to drop:");
            for (int i = 0; i < inventoryFill; i++) {
                mvwprintw(invlist, i+2, 2, "%c) %s", i+'a', inventory[i]->descr);
            }
            wrefresh(invlist);
            ch = wgetch(invlist);
            werase(invlist);
            destroy_win(invlist);
            int i = ch-'a';
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                for (int j = i + 1; j < inventoryFill; i++, j++) {
                    inventory[i] = inventory[j];
                }
                inventoryFill--;
                t->inInventory = false;
                sprite->under = t->badge;
                t->ypos = sprite->ypos;
                t->xpos = sprite->xpos;
            }
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

THING* locateThing(int ypos, int xpos) {
    THING* thing = things;
    while (thing != NULL) {
        if (ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        } else {
            thing = thing->next;
        }
    }
    return NULL;
}

THING* locateObject(int ypos, int xpos) {
    THING* thing = things;
    while (thing != NULL) {
        if ((thing->type == T_Item || thing->type == T_Structure)
                && ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        } else {
            thing = thing->next;
        }
    }
    return NULL;
}

THING* locateSprite(int ypos, int xpos) {
    THING* thing = things;
    while (thing != NULL) {
        if (thing->type == T_Sprite && ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        } else {
            thing = thing->next;
        }
    }
    return NULL;
}

void stepSprite(WINDOW* room, THING* sprite, chtype floor, int toY, int toX) {
    mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
    sprite->under = floor;
    sprite->ypos = toY;
    sprite->xpos = toX;
    present(sprite);
}

void combat(THING* sprite, int atY, int atX) {
    THING* s = locateSprite(atY, atX);
    int combatRoll = rand() % 6 + 1;
    if (combatRoll < sprite->attack) {
        mvaddstr(1, 0, "hit!");
        s->endurance--;
        if (s->endurance <= 0) {
            // s is killed
            s->badge = '%';
            present(s);
        }
    } else {
        mvaddstr(1, 0, "miss!");
    }
    clrtoeol();
}

void present(THING* sprite) {
    mvwaddch(sprite->room, sprite->ypos, sprite->xpos, sprite->badge);
    wrefresh(sprite->room);
}

// objects !  " # $ % & ' () * + , - .  / : ; < = > ?  [ \ ] ^ _ { | } ~
/*

 * */

THING* addArmour(WINDOW* win, int y, int x, const char* descr) {
    THING* t = newThing(win, T_Item, '&', y, x);
    t->descr = descr;
    return t;
}

THING* newThing(WINDOW* win, ThingType type, chtype badge, int y, int x) {
    THING* thing = malloc(sizeof(THING));
    if (thing == NULL) {
        mvaddstr(1, 0, "Out of memory, press any key to exit");
        getch();
        exit(1);
    }
    thing->type = type;
    thing->room = win;
    thing->badge = badge;
    thing->ypos = y;
    thing->xpos = x;
    thing->isEdible = false;
    thing->isPotable = false;
    thing->isEquippable = false;
    switch (badge) {
        case '$':
            thing->descr = "12 dollars";
            break;
        case '<':
            thing->descr = "stair";
            break;
        case '>':
            thing->descr = "stair";
            break;
        case '*':
            thing->descr = "leg of lamb";
            thing->isEdible = true;
            break;
        case ':':
            thing->descr = "gold ring";
            thing->isEquippable = true;
            break;
        case '!':
            thing->descr = "orange potion";
            thing->isPotable = true;
            break;
        case '@':
            thing->descr = "the player";
            thing->attack = 5;
            thing->endurance = 5;
            break;
        case 'a':
            thing->descr = "carnivorous ape";
            thing->attack = 4;
            thing->endurance = 3;
            break;
        case 'b':
            thing->descr = "barbarian";
            thing->attack = 3;
            thing->endurance = 4;
            break;
    }
    thing->inInventory = false;
    thing->next = things;
    things = thing;
    thing->under = ' ';
    present(thing);
    return thing;
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
}

void destroy_win(WINDOW *local_win) {	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

