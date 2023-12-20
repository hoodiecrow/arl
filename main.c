#include "main.h"
#include "maps.h"
#include "map9.h"

THING* things = NULL; // top pointer for thing linked list

THING* inventory[INVENTORY_SIZE];
int inventoryFill = 0;
bool allowedIndices[INVENTORY_SIZE];

THING* worn = NULL;
THING* right = NULL;
THING* left = NULL;
THING* player = NULL;

WINDOW* status;

static void getOpenLocation(WINDOW* win, int *y, int *x);

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

/* TODO enemies
 *                           Levels Aggressive?	Experience Award Gold Dropped
 * bat - symbol B --         1 - 7	sometimes	2
 * centaur - symbol C --     7 - 15	no	        15
 * emu - symbol E --         1 - 7	sometimes	2
 * hobgoblin - symbol H --   1 - 8	yes	        3
 * ice monster - symbol I -- 1 - 11	no	        5 You have a chance of being frozen with each successful melee hit. There is a small chance of dying of hypothermia as well. It is best to back away and shoot arrows or throw darts at it.
 * kestrel - symbol K --     1 - 5	sometimes	2
 * leprechaun - symbol L --  6 - 14	no	        21	             110 - 206 These enemies have a chance of stealing a fair portion of your gold and then disappearing ("your purse feels lighter"). This is similar to the nymph. It is best to either attack from long range or avoid them altogether as they are not aggressive. They will, however, drop gold when defeated
 * nymph - symbol N --       10 - 13	no	    ?  These enemies have a chance of stealing an item from your inventory and then disappearing ("she stole" and then the item). This is similar to the leprechaun. It is best to either attack from long range or avoid them altogether as they are not aggressive.
 * orc - symbol O --         2 - 13	sometimes	5 Interestingly, orcs will often move to stand guard over treasure.
 * quagga - symbol Q --      7 - 12	sometimes	20 A Quagga may drop an item upon death.
 * rattlesnake - symbol R -- 3 - 12	yes	        10 A rattlesnake's bite may weaken you (-1 current strength).
 * snake - symbol S --       1 - 9	sometimes	2
 * troll - symbol T --       14	    yes	        ?
 * venus fly-trap - symbol F 12 - 13	no	    91 When moved adjacent to, you become held by it until it is defeated. However, it is immobile so missile weapons are very effective.
 * wraith - symbol W --      14	    no	        55
 * yeti - symbol Y --        13	    yes	        50
 * zombie - symbol Z         5 - 14	sometimes	8
 */

// TODO traps
/*
 * bear trap -- A trap that prevents you from moving off the tile until you've managed to escape. When you first step into a bear trap, it reads: "you are caught in a bear trap". Every subsequent turn spent moving may result in your escape or the text: "you are still stuck in the bear trap".
 * poison dart trap -- A trap that shoots a dart at you which, if it hits, will deal damage and potentially reduce your strength. It reads: "a small dart just hit you in the shoulder".
 * rust trap -- A trap that applies a -1 modifier to the armor you are currently wearing. When you first step into a rust trap, it reads: "a gush of water hits you on the head" then: "your armor weakens".
 * sleeping gas trap -- A trap that puts you to sleep for a period of time (similar to the scroll of sleep). When you first step into a sleeping gas trap, it reads: "a strange white mist envelops you and you fall asleep".
 * teleport trap -- A trap that teleports you to a random location in the current dungeon level.
*/

/*TODO
You gain a new level when your experience points are equal to or greather than a power of two times ten. Your new max Hp is a random increase over your prevous Hp. Further statistics can be gathered in-game by pressing ^A.

Level	Experience Required	Max Hp
1	0	12
2	10	15 - 22
3	20	18 - 29
4	40	26 - 37
5	80	34 - 44
6	160	41 - 50
7	320	56 - 57
8	640	62 - 66
 */

/* TODO
 * At least one new enemy type may appear on each subsequent level of the dungeon past level one, though exactly which enemies spawn is randomly generated. A monster's symbol in bold designates his natural entry level.

 

Level	Quantities of Gold Found	Enemies Encountered
1	2 - 16	B, E, H, K, S
2	4 - 32	B, E, H, I, K, S
3	6 - 63	B, E, H, I, K, R, S
4	10 - 64	B, E, H, I, K, O, R, S
5	17 - 80	B, E, H, I, K, O, R, S, Z
6	14 - 90	B, E, H, I, L, O, R, S, Z
7	14 - 112	B, C, E, H, I, L, O, Q, R, S, Z
8	16 - 125	C, H, I, L, O, Q, R, S, Z
9	36 - 137	A, C, I, L?, O, Q?, R, S, Z
10	21 - 157	A?, C?, I, L, N, O, Q, R, Z?
11	27 - 106	A, C?, I, L, O?, Q, R?, Z
12	48 - 184	A?, C?, F, L, O, Q, R, Z
13	76 - 104	A?, C, F, L?, N, O, Y, Z?
14	38 - 179	A, C?, L, T, W, Z
15	114	C
 

There is a slim chance of discovering a "monster room". These rooms can be anywhere from normal sized to the size of the whole level. There will be many, many monsters present as well as a fair amount of loot. An enemy may show up in a monster room one level earlier than it would usually first appear.
*/

int dlevel = 1;

int main() {	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    refresh();

    srandom(time(NULL));
    //srand(time (NULL));

    WINDOW* map = create_newwin(37, 72, 2, 0);
    keypad(map, TRUE);

    status = create_newwin(19, 30, 20, 72);
    keypad(status, TRUE);

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

    curs_set(0);
    int y, x;
    for (int n = 0; n < random() % 5 + 1; n++) {
        addGold(map);
    }
    addRing(map);
    getOpenLocation(map, &y, &x);
    newThing(map, T_Item, '*', y, x);
    addPotion(map);
    getOpenLocation(map, &y, &x);
    newThing(map, T_Structure, '>', y, x);
    for (int n = 0; n < random() % 4 + 2; n++) {
        addMonster(map);
    }
    x = 10;
    y =  5;
    newThing(map, T_Sprite, '@', y, x);
    addArmour(map);
    addArmour(map);

    chtype ch = 0;
    while (ch != 'Q') {
        for (THING* thing = things; thing != NULL; thing = thing->next) {
            if (thing->type == T_Sprite) {
                ch = sprite_act(map, thing);
                werase(status);
                box(status, 0 , 0);
                mvwprintw(status, 1, 1, "Gold: %9d", player->gold);
                mvwprintw(status, 2, 1, "Constitution: %3d (%3d)", player->currConstitution, player->fullConstitution);
                mvwprintw(status, 3, 1, "Armour: %s (%d)", worn==NULL?"none":worn->descr, worn==NULL?0:worn->armour);
                mvwprintw(status, 4, 1, "RH: %s", right==NULL?"":right->ident);
                mvwprintw(status, 5, 1, "LH: %s", left==NULL?"":left->ident);
                mvwprintw(status, 6, 1, "Experience: %d", player->exp);
                wrefresh(status);

            }
            if (ch == 'Q')
                break;
        }

        refresh();
        wrefresh(map);
        wrefresh(status);
    }

    destroy_win(map);
	endwin();

	return 0;
}

typedef void (*ahptr)(WINDOW* room, THING* sprite);

void ah_1(WINDOW*, THING*); // sw
void ah_2(WINDOW*, THING*); // s
void ah_3(WINDOW*, THING*); // se
void ah_4(WINDOW*, THING*); // w
void ah_5(WINDOW*, THING*); // rest
void ah_6(WINDOW*, THING*); // e
void ah_7(WINDOW*, THING*); // nw
void ah_8(WINDOW*, THING*); // n
void ah_9(WINDOW*, THING*); // ne
void ah_E(WINDOW*, THING*); // equip
void ah_S(WINDOW*, THING*); // up stair, also <
void ah__(WINDOW*, THING*); // null action
void ah_c(WINDOW*, THING*); // close smth
void ah_d(WINDOW*, THING*); // drop, also .
void ah_e(WINDOW*, THING*); // eat
void ah_g(WINDOW*, THING*); // get
void ah_h(WINDOW*, THING*); // help screen, also ? and F1
void ah_i(WINDOW*, THING*); // inventory
void ah_l(WINDOW*, THING*); // look around (broken)
void ah_p(WINDOW*, THING*); // pick up, also ,
void ah_q(WINDOW*, THING*); // quaff
void ah_r(WINDOW*, THING*); // read
void ah_s(WINDOW*, THING*); // down stair, also >
void ah_u(WINDOW*, THING*); // use
void ah_w(WINDOW*, THING*); // wear
void ah_z(WINDOW*, THING*); // zap
void ahCS(WINDOW*, THING*); // Ctrl-S: save game

ahptr actionHandlers[256] = {
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ahCS, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah_p, &ah__, &ah_d, &ah__, &ah__, &ah_1,
    &ah_2, &ah_3, &ah_4, &ah_5, &ah_6, &ah_7, &ah_8, &ah_9, &ah__, &ah__,
    &ah_S, &ah__, &ah_s, &ah_h, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_E, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah_S, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_c, 
    &ah_d, &ah_e, &ah__, &ah_g, &ah_h, &ah_i, &ah__, &ah__, &ah_l, &ah__, 
    &ah__, &ah__, &ah_p, &ah_q, &ah_r, &ah_s, &ah__, &ah_u, &ah__, &ah_w, 
    &ah__, &ah__, &ah_z, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__,
};


int sprite_act(WINDOW* room, THING* sprite) {
    int ch;
    if (sprite->type != T_Sprite)
        return 0;
    if (sprite == player) {
        if (sprite->isDead)
            return 'Q';
        ch = wgetch(room);
        if (player->isConfused) {
            ch = 49 + random() % 8;
            player->confusionDuration--;
            if (player->confusionDuration == 0) {
                player->isConfused = false;
                mvaddstr(1, 0, "you feel less confused now"); clrtoeol();
            }
        }
    } else {
        if (sprite->isDead)
            return 0;
        int diffy = 0;
        int diffx = 0;
        int diry = 0;
        int dirx = 0;
        if (sprite->isAggressive) {
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
                ch = 49 + random() % 8;
            }
        } else {
            ch = 49 + random() % 8;
        }
    }
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        allowedIndices[i] = false;
    }
    if (sprite == player) {
        mvprintw(1, 0, "%d", ch); clrtoeol(); refresh();
    }
    switch (ch) {
        case 258: ch = '2'; break;
        case 259: ch = '8'; break;
        case 260: ch = '4'; break;
        case 261: ch = '5'; break;
        case 265: ch = 'h'; break;
    }
    actionHandlers[ch](room, sprite);
    return ch;
}

void ah__(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
}

void ahCS(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "save game");
    clrtoeol();
}

void ah_1(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    if (sprite->ypos < maxy-2 && sprite->xpos > 1) {
        attemptMove(room, sprite, +1, -1);
    }
}

void ah_2(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    if (sprite->ypos < maxy-2) {
        attemptMove(room, sprite, +1, +0);
    }
}

void ah_3(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    int maxx = getmaxx(room);
    if (sprite->ypos < maxy-2 && sprite->xpos < maxx-2) {
        attemptMove(room, sprite, +1, +1);
    }
}

void ah_4(WINDOW* room, THING* sprite) {
    if (sprite->xpos > 1) {
        attemptMove(room, sprite, +0, -1);
    }
}

void ah_5(WINDOW* room, THING* sprite) {
    (void)room;
    present(sprite);
}

void ah_6(WINDOW* room, THING* sprite) {
    int maxx = getmaxx(room);
    if (sprite->xpos < maxx-2) {
        attemptMove(room, sprite, +0, +1);
    }
}

void ah_7(WINDOW* room, THING* sprite) {
    if (sprite->ypos > 1 && sprite->xpos > 1) {
        attemptMove(room, sprite, -1, -1);
    }
}

void ah_8(WINDOW* room, THING* sprite) {
    if (sprite->ypos > 1) {
        attemptMove(room, sprite, -1, +0);
    }
}

void ah_9(WINDOW* room, THING* sprite) {
    int maxx = getmaxx(room);
    if (sprite->ypos > 1 && sprite->xpos < maxx-2) {
        attemptMove(room, sprite, -1, +1);
    }
}

void ah_c(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "close what?");
    clrtoeol();
}

void ah_e(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
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
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        //TODO eatEffect();
        dumpInventory(i);
    } else {
        mvaddstr(1, 0, "you can't eat that"); clrtoeol(); refresh();
    }
}

void ah_E(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
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
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        if (inventory[i]->glyph == ':') {
            mvaddstr(1, 0, "wear on right hand (R) or left hand (L)?");
            clrtoeol();
            refresh();
            switch (getch()) {
                case 'r': case 'R':
                    if (right != NULL) {
                        right->inInventory = true;
                    }
                    right = inventory[i];
                    equipEffect(i);
                    dumpInventory(i);
                    break;
                case 'l': case 'L':
                    if (left != NULL) {
                        left->inInventory = true;
                    }
                    left = inventory[i];
                    equipEffect(i);
                    dumpInventory(i);
                    break;
            }
        }
    } else {
        mvaddstr(1, 0, "you can't equip that"); clrtoeol(); refresh();
    }
}

void ah_g(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "get what?");
    clrtoeol();
}

void ah_h(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "help screen");
    clrtoeol();
}

void ah_i(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(15+3);
    mvwprintw(invlist, 1, 1, "%s", "You are carrying:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
    }
    endPopup(invlist);
}

void ah_l(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    // 1st pass: count the items
    int itemcount = 0;
    for (THING* t = things; t != NULL; t = t->next) {
        if (t->glyph != '@' && !t->inInventory && t != worn)
            itemcount++;
    }
    WINDOW* invlist = newPopup(inventoryFill+3);
    // 2nd pass: display the items
    int i = 1;
    mvwprintw(invlist, i++, 1, "%s", "Looking around, you see:");
    for (THING* t = things; t != NULL; t = t->next) {
        if (t->glyph != '@' && !t->inInventory && t != worn) {
            mvwprintw(invlist, i++, 1, "%s", t->descr);
        }
    }
    endPopup(invlist);
}

void ah_q(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
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
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        drinkEffect(i);
        dumpInventory(i);
    } else {
        mvaddstr(1, 0, "you can't drink that"); clrtoeol(); refresh();
    }
}

void ah_r(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to read:");
    for (int i = 0; i < inventoryFill; i++) {
        if ((inventory[i]->glyph == '~' || inventory[i]->glyph == '=')) {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        readEffect(i);
        dumpInventory(i);
    } else {
        mvaddstr(1, 0, "you can't read that"); clrtoeol(); refresh();
    }
}

void ah_s(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "you went down a stair");
    clrtoeol();
}

void ah_S(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "you went up a stair");
    clrtoeol();
}

void ah_u(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "use what?");
    clrtoeol();
}

void ah_w(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to wear:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '&') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        if (worn != NULL) {
            sprite->under = worn->glyph;
            worn->ypos = sprite->ypos;
            worn->xpos = sprite->xpos;
        }
        worn = inventory[i];
        wearEffect(i);
        dumpInventory(i);
    } else {
        mvaddstr(1, 0, "you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_z(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to zap with:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '\\' || inventory[i]->glyph == '/') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        // TODO find a target as well
        zapEffect(i);
    } else {
        mvaddstr(1, 0, "you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_p(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
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
}

void ah_d(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to drop:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (i >= 0 && i < inventoryFill) {
        THING* t = inventory[i];
        dumpInventory(i);
        sprite->under = t->glyph;
        t->ypos = sprite->ypos;
        t->xpos = sprite->xpos;
    }
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

void zapEffect(int i) {
    THING* t = inventory[i];
    mvaddstr(1, 0, t->descr); clrtoeol(); refresh();
    if (strcmp(t->ident, "wand of do nothing") == 0) {
        //When zapped, nothing happens. 
    } else if (strcmp(t->ident, "wand of fire") == 0) {
        // When zapped, fire shoots out at a target enemy. This is very powerful at lower dungeon levels. However, there is a chance of the fire bouncing off a target and returning to hit you. Of course, it can bounce off of you as well and return to hit your target. It is especially affective against ice monsters.
    } else if (strcmp(t->ident, "wand of haste monster") == 0) {
        // When zapped at a monster, that monster may be made quicker.
    } else if (strcmp(t->ident, "wand of invisibility") == 0) {
        // 
    } else if (strcmp(t->ident, "wand of teleport away") == 0) {
        // When zapped at a monster, the monster is teleported to another random part of the current dungeon level. It is identical to a staff of teleport away.
    } else if (strcmp(t->ident, "staff of drain life") == 0) {
        // When zapped, your life is drained by half (rounded down).
    } else if (strcmp(t->ident, "staff of slow monster") == 0) {
        // When zapped at a monster, that monster may be made slower.
    } else if (strcmp(t->ident, "staff of teleport away") == 0) {
        // When zapped at a monster, the monster is teleported to another random part of the current dungeon level. It is identical to a wand of teleport away.
    } else {
    }
    t->isIdentified = true;
}

void readEffect(int i) {
    THING* t = inventory[i];
    mvaddstr(1, 0, t->descr); clrtoeol(); refresh();
    if (strcmp(t->ident, "scroll of aggravate monster") == 0) {
        // Its affect reads: "you hear a high pitched humming noise". Monsters that may have been docile are now hostile towards you.
    } else if (strcmp(t->ident, "scroll of confuse monster") == 0) {
        // Its affect reads: "your hands glow red for a moment". When you strike enemies using a melee weapon there is a chance that they will become confused.
    } else if (strcmp(t->ident, "scroll of create monster") == 0) {
        // Just like its name says, it creates a random monster (suitable for that dungeon level) adjacent to you. This monster will be hostile.
    } else if (strcmp(t->ident, "scroll of enchant armour") == 0) {
        // Its affect reads: "your armor glows blue for a moment". It increases the bonus on your armor by 1. If your armor was cursed (e.g. -3), it will no longer be so and the penalty for wearing it will be one less (e.g. -2).
    } else if (strcmp(t->ident, "scroll of enchant weapon") == 0) {
        // Its affect reads: "your weapon glows blue for a moment". It increases the bonus on your weapon by +0,+1. If your weapon was cursed, it will no longer be so and the penalty for weilding it will be one less.
    } else if (strcmp(t->ident, "scroll of hold monster") == 0) {
        // Its affect reads: "the monster freezes". It keeps monsters from moving, obviously.
    } else if (strcmp(t->ident, "scroll of identify") == 0) {
        // It identifies another item of your choosing when used.
    } else if (strcmp(t->ident, "scroll of magic mapping") == 0) {
        // Its affect reads: "this scroll seems to have a map on it". It reveals all the rooms, corridors, traps, and stairs down on that dungeon level. It does not show enemies or items.
    } else if (strcmp(t->ident, "scroll of protect armour") == 0) {
        // Its affect reads: "your armor is covered by a shimmering gold shield".
    } else if (strcmp(t->ident, "scroll of remove curse") == 0) {
        // Its affect reads: "you feel as though someone is watching over you".
    } else if (strcmp(t->ident, "scroll of scare monster") == 0) {
        // Its affect reads: "you hear a maniacal laughter in the distance". It has a chance of scaring aggressive monsters away.
    } else if (strcmp(t->ident, "scroll of sleep") == 0) {
        // Its affect reads: "you fall asleep". Obviously, you fall asleep for some amount of time (similar to the sleeping gas trap).
    } else if (strcmp(t->ident, "scroll of teleportation") == 0) {
        // It teleports you to a random location in the current dungeon level.
    } else {
    }
    t->isIdentified = true;
}

void equipEffect(int i) {
    // TODO Rings may be worn on either the left or right hand with no change in effect. While worn, however, they will make you hungrier quicker.
    THING* t = inventory[i];
    mvaddstr(1, 0, t->descr); clrtoeol(); refresh();
    if (strcmp(t->ident, "ring of adornment") == 0) {
        // no special effect
    } else if (strcmp(t->ident, "ring of dexterity") == 0) {
        // When put on, your dexterity is adjusted by the ring's stated modifier.
    } else if (strcmp(t->ident, "ring of maintain armour") == 0) {
        // When put on, your armor cannot be damaged by traps or enemies (such as the aquator).
    } else if (strcmp(t->ident, "ring of see invisible") == 0) {
        // When put on, you will be able to see invisible enemies.
    } else if (strcmp(t->ident, "ring of teleportation") == 0) {
        // When put on, you will be randomly teleported to another location in the current dungeon level every few actions. This is a cursed ring and cannot be removed until uncursed.
    } else {
    }
    t->isIdentified = true;

}

void wearEffect(int i) {
    THING* t = inventory[i];
    mvaddstr(1, 0, t->descr); clrtoeol(); refresh();
    if (strcmp(t->descr, "banded mail") == 0) {
        t->armour = 6;
    } else if (strcmp(t->descr, "chain mail") == 0) {
        t->armour = 5;
    } else if (strcmp(t->descr, "leather armour") == 0) {
        t->armour = 2;
    } else if (strcmp(t->descr, "plate mail") == 0) {
        t->armour = 7;
    } else if (strcmp(t->descr, "ring mail") == 0) {
        t->armour = 3;
    } else if (strcmp(t->descr, "scale mail") == 0) {
        t->armour = 4;
    } else if (strcmp(t->descr, "splint mail") == 0) {
        t->armour = 6;
    } else {
        t->armour = 0;
    }
    t->isIdentified = true;
    // TODO lesser or greater quality variants
}

void drinkEffect(int i) {
    THING* t = inventory[i];
    mvaddstr(1, 0, t->ident); clrtoeol(); refresh();
    t->isIdentified = true;
    //TODO replace description
    //TODO timer for (some) effects
    if (strcmp(t->ident, "potion of blindness") == 0) {
        // Its effect reads: "a cloak of darkness falls around you". You are unable to see enemies, items, and the dungeon around you.
    } else if (strcmp(t->ident, "potion of confusion") == 0) {
        // Its effect reads: "you feel confused". For a short amount of time, you are unable to move in the direction you press. When its effect wears off it reads: "you feel less confused now".
        mvaddstr(1, 0, "you feel confused"); clrtoeol();
        player->isConfused = true;
        player->confusionDuration = 12;
    } else if (strcmp(t->ident, "potion of detect monster") == 0) {
        // When read [sic] no text appears. However, it reveals all monsters on the current dungeon level.
    } else if (strcmp(t->ident, "potion of detect things") == 0) {
        // When read no text appears. However, it reveals all items and gold present in the current dungeon level.
    } else if (strcmp(t->ident, "potion of extra healing") == 0) {
        // Its effect reads: "you begin to feel much better". It restores a very large number of Hp (27).
        mvaddstr(1, 0, "you begin to feel much better"); clrtoeol();
        player->currConstitution += 27;
        if (player->currConstitution > player->fullConstitution)
            player->currConstitution = player->fullConstitution;
    } else if (strcmp(t->ident, "potion of hallucination") == 0) {
        // Its effect reads: "oh wow, everything seems so cosmic". When quaffed all enemies and items will change symbol from turn to turn. When its effect wears off it reads: "everything looks SO boring now".
    } else if (strcmp(t->ident, "potion of haste self") == 0) {
        //  Its effect reads: "you feel yourself moving much faster". It may appear as though you are moving at your normal speed but you can sometimes move twice before a monster moves once.
    } else if (strcmp(t->ident, "potion of healing") == 0) {
        // Its effect reads: "you begin to feel better". It restores a number of Hp (13).
        mvaddstr(1, 0, "you begin to feel better"); clrtoeol();
        player->currConstitution += 13;
        if (player->currConstitution > player->fullConstitution)
            player->currConstitution = player->fullConstitution;
    } else if (strcmp(t->ident, "potion of increase strength") == 0) {
        // Its effect reads: "you feel stronger now, what bulging muscles!". When quaffed your current strength increases by one (like a potion of restore strength), however, if you strength is at max, your maximum strength increases by one.
    } else if (strcmp(t->ident, "potion of levitation") == 0) {
        // Its effect reads: "you start to float in the air". While levitating you cannot fall into traps or pickup items.
    } else if (strcmp(t->ident, "potion of poison") == 0) {
        // Its effect reads: "you feel very sick now". It reduces your current strength by 3.
    } else if (strcmp(t->ident, "potion of raise level") == 0) {
        //  Its effect reads: "you suddenly feel much more skillful". You instantly attain the next level (Exp automatically adjusted).
    } else if (strcmp(t->ident, "potion of restore strength") == 0) {
        // Its effect reads: "this tastes great, you feel warm all over". Your current strength increases by one up to your maximum strength.
    } else if (strcmp(t->ident, "potion of see invisible") == 0) {
        // Its effect reads: "hmm, this potion tastes like slime-mold juice". Presumably it lets you see invisible things...
    } else {
        // shouldn't happen
    }
    t->isIdentified = true;
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

static void freeObject(THING* o) {
    if (things == o) {
        things = o->next;
    } else {
        THING* t;
        for (t = things; t->next != o; t = t->next)
            ;
        t->next = o->next;
    }
    free(o);
}

static void playerStepsOnGold(int toY, int toX) {
    THING* o = locateObject(toY, toX);
    if (o != NULL) {
        player->gold += o->gold;
        //mvwprintw(status, 1, 1, "Gold: %9d", player->gold);
        freeObject(o);
    }
}

void stepSprite(WINDOW* room, THING* sprite, chtype floor, int toY, int toX) {
    mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
    if (floor == '$' && sprite == player) {
        playerStepsOnGold(toY, toX);
    } else {
        sprite->under = floor;
    }
    sprite->ypos = toY;
    sprite->xpos = toX;
    present(sprite);
}

void combat(THING* sprite, int atY, int atX) {
    THING* other = locateSprite(atY, atX);
    int combatRoll = random() % 6 + 1;
    if (combatRoll < sprite->attack) {
        mvaddstr(1, 0, "hit!");
        other->currConstitution--;
        if (other->currConstitution <= 0) {
            // other is killed
            other->isDead = true;
            other->glyph = '%';
            present(other);
            sprite->exp += other->expAward;
        }
        //TODO could attempt to escape instead
        other->isAggressive = true;
    } else {
        mvaddstr(1, 0, "miss!");
    }
    clrtoeol();
}

void present(THING* sprite) {
    mvwaddch(sprite->room, sprite->ypos, sprite->xpos, sprite->glyph);
    wrefresh(sprite->room);
}

static void getOpenLocation(WINDOW* win, int *y, int *x) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    int ry = random() % maxy;
    int rx = random() % maxx;
    while (mvwinch(win, ry, rx) != ' ') {
        ry = random() % maxy;
        rx = random() % maxx;
    }
    *y = ry;
    *x = rx;
}

THING* addMonster(WINDOW* win) {
    int y, x, i;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Sprite, 'X', y, x);
    const char *aggr;
    if (dlevel == 1) 
        i = random() % 6;
    else if (dlevel == 1) 
        i = random() % 7;
    else
        i = 0;
    switch (i) {
        case 0:
            t->descr = "bat";
            t->glyph = 'B';
            aggr = "sometimes";
            t->attack = 1;
            t->fullConstitution = 1;
            t->expAward = 2;
            break;
        case 1:
            t->descr = "emu";
            t->glyph = 'E';
            aggr = "sometimes";
            t->attack = 2;
            t->fullConstitution = 1;
            t->expAward = 2;
            break;
        case 2:
            t->descr = "goblin";
            t->glyph = 'G';
            aggr = "sometimes";
            t->attack = 2;
            t->fullConstitution = 2;
            t->expAward = 2;
            break;
        case 3:
            t->descr = "hobgoblin";
            t->glyph = 'H';
            aggr = "yes";
            t->attack = 3;
            t->fullConstitution = 3;
            t->expAward = 3;
            break;
        case 4:
            t->descr = "kestrel";
            t->glyph = 'K';
            aggr = "sometimes";
            t->attack = 1;
            t->fullConstitution = 2;
            t->expAward = 2;
            break;
        case 5:
            t->descr = "snake";
            t->glyph = 'S';
            aggr = "sometimes";
            t->attack = 1;
            t->fullConstitution = 1;
            t->expAward = 2;
            break;
        case 6:
            t->descr = "ice monster";
            t->glyph = 'I';
            aggr = "no";
            t->attack = 4;
            t->fullConstitution = 4;
            t->expAward = 5;
            break;
    }
    t->currConstitution = t->fullConstitution;
    if (strcmp(aggr, "sometimes") == 0) {
        t->isAggressive = random()%2==1?true:false;
    } else if (strcmp(aggr, "no") == 0) {
        t->isAggressive = false;
    } else if (strcmp(aggr, "yes") == 0) {
        t->isAggressive = true;
    }
    return t;
}

THING* addArmour(WINDOW* win) {
    int i = random() % 7;
    const char *descrs[] = {
        "banded mail",
        "chain mail",
        "leather armour",
        "plate mail",
        "ring mail",
        "scale mail",
        "splint mail",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '&', y, x);
    t->descr = descrs[i];
    return t;
}

THING* addWand(WINDOW* win) {
    int i = random() % 6;
    int j = random() % 5;
    const char *descrs[] = {
        "copper wand",
        "gold wand",
        "iron wand",
        "nickel wand",
        "silver wand",
        "titanium wand",
    };
    const char *idents[] = {
        "wand of do nothing",
        "wand of fire",
        "wand of haste monster",
        "wand of invisibility",
        "wand of teleport away",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '/', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->isIdentified = false;
    t->ident = idents[j];
    return t;
}

THING* addStaff(WINDOW* win) {
    int i = random() % 7;
    int j = random() % 5;
    const char *descrs[] = {
        "birch staff",
        "cedar staff",
        "elm staff",
        "maple staff",
        "redwood staff",
        "teak staff",
        "walnut staff",
        "plain staff",
    };
    const char *idents[] = {
        "staff of drain life",
        "staff of slow monster",
        "staff of teleport away",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '/', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->isIdentified = false;
    t->ident = idents[j];
    return t;
}

THING* addScroll(WINDOW* win) {
    int i = random() % 13;
    const char *idents[] = {
        "scroll of aggravate monster",
        "scroll of confuse monster",
        "scroll of create monster",
        "scroll of enchant armor",
        "scroll of enchant weapon",
        "scroll of hold monster",
        "scroll of identify",
        "scroll of magic mapping",
        "scroll of protect armor",
        "scroll of remove curse",
        "scroll of scare monster",
        "scroll of sleep",
        "scroll of teleportation",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '~', y, x);
    t->descr = "a magical scroll";
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->isIdentified = false;
    t->ident = idents[i];
    return t;
}

THING* addRing(WINDOW* win) {
    int i = random() % 5;
    const char *descrs[] = {
        "diamond ring",
        "emerald ring",
        "ruby ring",
        "opal ring",
        "sapphire ring",
    };
    const char *idents[] = {
        "ring of adornment",
        "ring of dexterity",
        "ring of maintain armor",
        "ring of see invisible",
        "ring of teleportation",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, ':', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->isIdentified = false;
    t->ident = idents[i];
    return t;
}

THING* addPotion(WINDOW* win) {
    int i = random() % 14;
    const char *descrs[] = {
        "beige potion",
        "black potion",
        "blue potion",
        "brown potion",
        "burgundy potion",
        "clear potion",
        "green potion",
        "grey potion",
        "pink potion",
        "plaid potion",
        "purple potion",
        "red potion",
        "white potion",
        "yellow potion",
    };
    const char *idents[] = {
        "potion of blindness",
        "potion of confusion",
        "potion of detect monster",
        "potion of detect things",
        "potion of extra healing",
        "potion of hallucination",
        "potion of haste self",
        "potion of healing",
        "potion of increase strength",
        "potion of levitation",
        "potion of poison",
        "potion of raise level",
        "potion of restore strength",
        "potion of see invisible",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '!', y, x);
    t->descr = descrs[i];
    t->isPotable = true;
    //TODO if dropped after being identified, should be true
    t->isIdentified = false;
    t->ident = idents[i];
    return t;
}

THING* addGold(WINDOW* win) {
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '$', y, x);
    t->gold = 2 + random() % 14;
    return t;
}

THING* newThing(WINDOW* win, ThingType type, chtype glyph, int y, int x) {
    THING* thing = malloc(sizeof(THING));
    if (thing == NULL) {
        mvaddstr(1, 0, "Out of memory, press any key to exit");
        refresh();
        getch();
        exit(1);
    }
    if (glyph == '@')
        player = thing;
    thing->type = type;
    thing->room = win;
    thing->glyph = glyph;
    thing->ypos = y;
    thing->xpos = x;
    thing->isEdible = false;
    thing->isPotable = false;
    thing->isEquippable = false;
    thing->attack = 0;
    thing->fullConstitution = 0;
    thing->isDead = false;
    thing->exp = 0;
    switch (glyph) {
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
        case '@':
            thing->descr = "the player";
            thing->attack = 5;
            thing->fullConstitution = 5;
            break;
    }
    thing->gold = 0;
    thing->currConstitution = thing->fullConstitution;
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

