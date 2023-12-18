#include "main.h"
#include "maps.h"
#include "map9.h"

THING* things = NULL; // top pointer for thing linked list

THING* inventory[INVENTORY_SIZE];
int inventoryFill = 0;
bool allowedIndices[INVENTORY_SIZE];

THING* worn = NULL;
THING* player = NULL;

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

    srand(time (NULL));

    WINDOW* map = create_newwin(37, 72, 2, 0);
    keypad(map, TRUE);

    //genMap(35, 90, 35, 5, 1, 10);
    genMap9(35, 70);

    //showMap(map);
    showMap9(map);
    wrefresh(map);
            
#if 0
    int height = 16;
    int width = 20;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    WINDOW *room = create_newwin(height, width, starty, startx);
    keypad(room, TRUE);
#endif

    //TODO drop things in open space
    curs_set(0);
    newThing(map, T_Item, '$', 2, 4);
    newThing(map, T_Item, ':', 6, 10);
    newThing(map, T_Item, '*', 12, 9);
    newThing(map, T_Item, '!', 10, 10);
    newThing(map, T_Structure, '>', 4, 12);
    addMonster(map, "ape", 4, 3);
    addMonster(map, "ape", 4, 3);
    addMonster(map, "barbarian", 3, 4);
    int x = 10;
    int y =  5;
    newThing(map, T_Sprite, '@', y, x);
    addArmour(map, 12, 10, "yellow jammies");
    addArmour(map, 2, 6, "green jammies");

    chtype ch = 0;
    while (ch != 'Q') {
        for (THING* thing = things; thing != NULL; thing = thing->next) {
            if (thing->type == T_Sprite) {
                ch = sprite_act(map, thing);
            }
            if (ch == 'Q')
                break;
        }

        refresh();
        wrefresh(map);
    }

    destroy_win(map);
	endwin();

	return 0;
}

int sprite_act(WINDOW* room, THING* sprite) {
    int ch;
    if (sprite->type != T_Sprite)
        return 0;
    if (sprite->constitution <= 0) {
        if (sprite == player) 
            return 'Q';
        else
            return 0;
    }
    int maxy = getmaxy(room);
    int maxx = getmaxx(room);
    if (sprite->badge == '@') {
        ch = wgetch(room);
    } else {
        //TODO not totally random movement, e.g. moving towards player
        int diffy = 0;
        int diffx = 0;
        int diry = 0;
        int dirx = 0;
        if (player->ypos > sprite->ypos) {
            diffy = player->ypos - sprite->ypos;
            diry = 1;
        } else if (player->ypos == sprite->ypos) {
            diry = 0;
        } else {
            diffy = sprite->ypos - player->ypos;
            diry = -1;
        }
        if (player->xpos > sprite->xpos) {
            diffx = player->xpos - sprite->xpos;
            dirx = 1;
        } else if (player->xpos == sprite->xpos) {
            dirx = 0;
        } else {
            diffx = sprite->xpos - player->xpos;
            dirx = -1;
        }
        if (diffy < 3 && diffx < 3) {
            if (diry == 1 && dirx == -1)
                ch = '1';
            else if (diry == 1 && dirx == 0)
                ch = '2';
            else if (diry == 1 && dirx == 1)
                ch = '3';
            else if (diry == 0 && dirx == -1)
                ch = '4';
            else if (diry == 0 && dirx == 1)
                ch = '6';
            else if (diry == -1 && dirx == -1)
                ch = '7';
            else if (diry == -1 && dirx == 0)
                ch = '8';
            else if (diry == -1 && dirx == 1)
                ch = '9';
            else
                ch = '5'; // won't happen
        } else {
            ch = 49 + rand() % 8;
        }
    }
    if (ch == '1') {
        if (sprite->ypos < maxy-2 && sprite->xpos > 1) {
            attemptMove(room, sprite, +1, -1);
        }
    } else if (ch == '2') {
        if (sprite->ypos < maxy-2) {
            attemptMove(room, sprite, +1, +0);
        }
    } else if (ch == '3') {
        if (sprite->ypos < maxy-2 && sprite->xpos < maxx-2) {
            attemptMove(room, sprite, +1, +1);
        }
    } else if (ch == '4') {
        if (sprite->xpos > 1) {
            attemptMove(room, sprite, +0, -1);
        }
    } else if (ch == '5') {
        present(sprite);
    } else if (ch == '6') {
        if (sprite->xpos < maxx-2) {
            attemptMove(room, sprite, +0, +1);
        }
    } else if (ch == '7') {
        if (sprite->ypos > 1 && sprite->xpos > 1) {
            attemptMove(room, sprite, -1, -1);
        }
    } else if (ch == '8') {
        if (sprite->ypos > 1) {
            attemptMove(room, sprite, -1, +0);
        }
    } else if (ch == '9') {
        if (sprite->ypos > 1 && sprite->xpos < maxx-2) {
            attemptMove(room, sprite, -1, +1);
        }
    }
    if (sprite->badge == '@') {
        if (ch == 'c') {
            mvaddstr(1, 0, "close what?");
            clrtoeol();
        } else if (ch == 'e') {
            //                     ((( e )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to eat:");
            for (int i = 0; i < inventoryFill; i++) {
                if (inventory[i]->isEdible) {
                    mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
                    allowedIndices[i] = true;
                } else {
                    allowedIndices[i] = false;
                }
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (allowedIndices[i]) {
                //TODO effect
                dumpInventory(i);
            } else {
                mvaddstr(1, 0, "you can't eat that"); clrtoeol(); refresh();
            }
        } else if (ch == 'E') {
            //                     ((( E )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to equip:");
            for (int i = 0; i < inventoryFill; i++) {
                if (inventory[i]->isEquippable) {
                    mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
                    allowedIndices[i] = true;
                } else {
                    allowedIndices[i] = false;
                }
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (allowedIndices[i]) {
                //TODO effect
                dumpInventory(i);
            } else {
                mvaddstr(1, 0, "you can't equip that"); clrtoeol(); refresh();
            }
        } else if (ch == 'g') {
            //                     ((( g )))
            mvaddstr(1, 0, "get what?");
            clrtoeol();
        } else if (ch == 'i') {
            //                     ((( i )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "You are carrying:");
            for (int i = 0; i < inventoryFill; i++) {
                mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            }
            ch = endPopup(invlist);
        } else if (ch == 'l') {
            //                     ((( l )))
            // 1st pass: count the items
            int itemcount = 0;
            for (THING* t = things; t != NULL; t = t->next) {
                if (t->badge != '@' && !t->inInventory && t != worn)
                    itemcount++;
            }
            WINDOW* invlist = newPopup(inventoryFill+3);
            // 2nd pass: display the items
            int i = 1;
            mvwprintw(invlist, i++, 1, "%s", "Looking around, you see:");
            for (THING* t = things; t != NULL; t = t->next) {
                if (t->badge != '@' && !t->inInventory && t != worn) {
                    mvwprintw(invlist, i++, 1, "%s", t->descr);
                }
            }
            ch = endPopup(invlist);
        } else if (ch == 'q') {
            //                     ((( q )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to drink:");
            for (int i = 0; i < inventoryFill; i++) {
                if (inventory[i]->isPotable) {
                    mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
                    allowedIndices[i] = true;
                } else {
                    allowedIndices[i] = false;
                }
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (allowedIndices[i]) {
                //TODO effect
                dumpInventory(i);
            } else {
                mvaddstr(1, 0, "you can't drink that"); clrtoeol(); refresh();
            }
        } else if (ch == 'r') {
            //                     ((( r )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to read:");
            for (int i = 0; i < inventoryFill; i++) {
                if ((inventory[i]->badge == '~' || inventory[i]->badge == '=')) {
                    mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
                    allowedIndices[i] = true;
                } else {
                    allowedIndices[i] = false;
                }
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (allowedIndices[i]) {
                //TODO effect
                dumpInventory(i);
            } else {
                mvaddstr(1, 0, "you can't read that"); clrtoeol(); refresh();
            }
        } else if (ch == 'u') {
            //                     ((( u )))
            mvaddstr(1, 0, "use what?");
            clrtoeol();
        } else if (ch == 'w') {
            //                     ((( w )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to wear:");
            for (int i = 0; i < inventoryFill; i++) {
                if (inventory[i]->badge == '&') {
                    mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
                    allowedIndices[i] = true;
                } else {
                    allowedIndices[i] = false;
                }
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (allowedIndices[i]) {
                if (worn != NULL) {
                    sprite->under = worn->badge;
                    worn->ypos = sprite->ypos;
                    worn->xpos = sprite->xpos;
                }
                worn = inventory[i];
                dumpInventory(i);
            } else {
                mvaddstr(1, 0, "you can't wear that"); clrtoeol(); refresh();
            }
        } else if (ch == 'z') {
            //                     ((( z )))
            mvaddstr(1, 0, "zap in which direction?");
            clrtoeol();
        } else if (ch == ',') {
            //                     ((( , )))
            if (sprite->under == ' ') {
                mvaddstr(1, 0, "there's nothing to pick up");
            } else if (inventoryFill == INVENTORY_SIZE) {
                mvaddstr(1, 0, "you can't carry more items");
            } else {
                THING* t = locateObject(sprite->ypos, sprite->xpos);
                if (t == NULL) {
                    mvaddstr(1, 0, "there's nothing to pick up");
                } else if (t->type == T_Structure) {
                    mvaddstr(1, 0, "you can't pick that up");
                } else {
                    inventory[inventoryFill++] = t;
                    mvprintw(1, 0, "you picked up: %s", t->descr);
                    //TODO multiple items in same space
                    sprite->under = ' ';
                    t->inInventory = true;
                    t->ypos = -1;
                    t->xpos = -1;
                }
            }
            clrtoeol();
        } else if (ch == '.') {
            //                     ((( . )))
            WINDOW* invlist = newPopup(inventoryFill+3);
            mvwprintw(invlist, 1, 1, "%s", "What do you want to drop:");
            for (int i = 0; i < inventoryFill; i++) {
                mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            }
            ch = endPopup(invlist);
            int i = ch-'a';
            if (i >= 0 && i < inventoryFill) {
                THING* t = inventory[i];
                dumpInventory(i);
                //TODO drop more than one item
                sprite->under = t->badge;
                t->ypos = sprite->ypos;
                t->xpos = sprite->xpos;
            }
        } else if (ch == '<') {
            //                     ((( < )))
            mvaddstr(1, 0, "you climbed up a stair");
            clrtoeol();
        } else if (ch == '>') {
            //                     ((( > )))
            mvaddstr(1, 0, "you went down a stair");
            clrtoeol();
        } else if (ch == '@') {
            //                     ((( @ )))
            mvaddstr(1, 0, "character screen");
            clrtoeol();
        } else if (ch == 19) {
            //                     ((( C^S )))
            mvaddstr(1, 0, "save game");
            clrtoeol();
        } else if (ch == KEY_F(1)) {
            //                     ((( F1 )))
            mvaddstr(1, 0, "help screen");
            clrtoeol();
            // more keys at roguebasin
        } else 
            ;
    }
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        allowedIndices[i] = false;
    }
    return ch;
}

WINDOW* newPopup(int lines) {
    WINDOW* win = create_newwin(lines, 30, 2, 72);
    return win;
}

chtype endPopup(WINDOW* win) {
    wrefresh(win);
    chtype ch = wgetch(win);
    werase(win);
    destroy_win(win);
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

void dumpInventory(int i) {
    inventory[i]->inInventory = false;
    for (int j = i + 1; j < inventoryFill; i++, j++) {
        inventory[i] = inventory[j];
    }
    inventoryFill--;
}

void attemptMove(WINDOW* room, THING* sprite, int incrY, int incrX) {
    int toY = sprite->ypos+incrY;
    int toX = sprite->xpos+incrX;
    chtype floor = mvwinch(room, toY, toX);
    if (floor == '@' || isalpha(floor)) {
        combat(sprite, toY, toX);
    } else if (floor == '#') {
        // a wall
    } else if (floor == ' ' || ispunct(floor)) {
        stepSprite(room, sprite, floor, toY, toX);
    }
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
        s->constitution--;
        if (s->constitution <= 0) {
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

THING* addMonster(WINDOW* win, const char* descr, int atk, int con) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    int y = rand() % maxy;
    int x = rand() % maxx;
    while (mvwinch(win, y, x) != ' ') {
        y = rand() % maxy;
        x = rand() % maxx;
    }
    THING* t = newThing(win, T_Sprite, descr[0], y, x);
    t->descr = descr;
    t->attack = atk;
    t->constitution = con;
    return t;
}

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
    if (badge == '@')
        player = thing;
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
            thing->constitution = 5;
            break;
        case 'a':
            thing->descr = "carnivorous ape";
            thing->attack = 4;
            thing->constitution = 3;
            break;
        case 'b':
            thing->descr = "barbarian";
            thing->attack = 3;
            thing->constitution = 4;
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

