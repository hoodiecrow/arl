#include "main.h"
#include "maps.h"
#include "ah.h"
#include "map9.h"

THING* things = NULL; // top pointer for thing linked list

THING* inventory[INVENTORY_SIZE];
int inventoryFill = 0;
bool allowedIndices[INVENTORY_SIZE];

THING* worn = NULL;
THING* right = NULL;
THING* left = NULL;
THING* wielded = NULL;
THING* player = NULL;
int deltaY = 0;
int deltaX = 0;

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

int rnd(int i) {
    // take an integer and return random() % that integer: i=0 yields 0
    return i == 0 ? 0 : random() % i;
}

int dice(int n, int s) {
    // take a number n of dice of size s, return a dice roll
    // n=number of dice
    // s=size of the die
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += rnd(s) + 1;
    }
    return total;
}

int dice2(const char* code) {
    // take a dice code, return a dice roll
    int n, s;
    int r = sscanf(code, "%dd%d", &n, &s);
    if (r != 2) {
        mvprintw(1, 0, "can't scan \"%s\"", code);
        clrtoeol();
        refresh();
        getch();
        exit(1);
    }
    return dice(n, s);
}

int expForLevel(int i) {
    // take a level number, return the exp needed for that level
    switch (i) {
        case 1: return 0;
        case 2: return 10;
        case 3: return 20;
        case 4: return 40;
        case 5: return 80;
        case 6: return 160;
        case 7: return 320;
        case 8: return 640;
        default: return -1;
    }
}

int hpIncrForLevel(int i) {
    // take a level number, return the HP increase for that level
    switch (i) {
        case 1: return 0;
        case 2: return 3 + rnd(22-15);
        case 3: return 3 + rnd(29-18);
        case 4: return 8 + rnd(37-26);
        case 5: return 8 + rnd(44-34);
        case 6: return 7 + rnd(50-41);
        case 7: return 15+ rnd(57-56); // weird
        case 8: return 6 + rnd(66-62);
        default: return -1;
    }
}

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

void initGame();
void runGame();

int dlevel = 1;
WINDOW* map;

int main() {	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    srandom(time(NULL));

    map = create_newwin(37, 72, 2, 0);
    keypad(map, TRUE);

    //genMap(35, 90, 35, 5, 1, 10);
    genMap9(35, 70);

    //showMap(map);
    showMap9(map);
    wrefresh(map);
            
    curs_set(0);
    
    initGame();
    runGame();

    destroy_win(map);
	endwin();

	return 0;
}

void initGame() {
    int y, x;
    for (int n = 0; n < rnd(5) + 1; n++) {
        addGold(map);
    }
    addRing();
    addWeapon();
    addScroll();
    addWand();
    addStaff();
    present(place(newThing(T_Item, '*')));
    addPotion();
    present(place(newThing(T_Structure, '>')));
    for (int n = 0; n < rnd(4) + 2; n++) {
        addMonster(0);
    }
    x = 10;
    y =  5;
    present(place(newThing(T_Sprite, '@')));
    addArmour();
    addArmour();
}

void runGame() {
    chtype ch = 0;
    while (ch != 'Q') {
        // status line
        mvprintw(LINES-1, 0, "Level: %d  Gold: %-5d  HP: %d(%d)  Str: %-2d  AC: %-2d  Level/Exp: %d/%ld",
                dlevel, player->gold, player->stats->currHp, player->stats->fullHp, 
                player->stats->currStrength, worn==NULL?0:player->armour,
                player->stats->level, player->stats->exp); clrtoeol(); refresh();

        if (player->isLevitating) {
            player->levitationDuration--;
        }
        if (player->isHasted) {
            ch = player_act(player);
            if (ch == 'Q')
                break;
            player->hasteDuration--;
        }
        if (player->isConfused) {
            player->confusionDuration--;
        }
        if (player->isAsleep) {
            player->sleepDuration--;
        }
        if (player->isHallucinating) {
            player->hallucinationDuration--;
        }
        if (player->isBlind) {
            player->blindnessDuration--;
        }
        if (player->wearingTeleportRing) {
            player->teleportationCycle--;
            if (player->teleportationCycle == 0) {
                mvwaddch(player->room, player->ypos, player->xpos, player->under);
                place(player);
                present(player);
            }
        }
        if (player->isInjured) {
            if (player->healingCycle <= 0)
                player->healingCycle = 10;
            if (player->isInCombat) {
                player->isInCombat = false;
            } else {
                player->healingCycle--;
            }
        }
        ch = player_act(player);
        for (THING* thing = things; ch != 'Q' && thing != NULL; thing = thing->next) {
            if (thing->type == T_Sprite && thing != player) {
                sprite_act(thing);
            }
        }

        if (player->isHasted && player->hasteDuration <= 0) {
            player->isHasted = false;
        }
        if (player->isLevitating && player->levitationDuration <= 0) {
            player->isLevitating = false;
            msg("your feet touch the ground again");
        }
        if (player->isConfused && player->confusionDuration <= 0) {
            player->isConfused = false;
            msg("you feel less confused now");
        }
        if (player->isAsleep && player->sleepDuration <= 0) {
            player->isAsleep = false;
            msg("you wake up");
        }
        if (player->isHallucinating && player->hallucinationDuration <= 0) {
            player->isHallucinating = false;
        }
        if (player->isBlind && player->blindnessDuration <= 0) {
            player->isBlind = false;
        }
        if (player->wearingTeleportRing && player->teleportationCycle <= 0) {
            player->teleportationCycle = rnd(6) + 1;
        }
        if (player->isInjured) {
            if (player->healingCycle <= 0) {
                if (player->stats->currHp < player->stats->fullHp) {
                    player->stats->currHp++;
                } else if (player->stats->currHp == player->stats->fullHp) {
                    player->isInjured = false;
                }
            }
        }
        refresh();
        wrefresh(map);
    }
}

static void stepSprite(THING* sprite, int toY, int toX) {
    // take a sprite and a pair of coords, and move there
    mvwaddch(sprite->room, sprite->ypos, sprite->xpos, sprite->under);
    chtype floor = mvwinch(sprite->room, toY, toX);
    if (floor == '$' && sprite == player) {
        THING* o = locateObject(toY, toX);
        if (o != NULL) {
            player->gold += o->gold;
            freeObject(o);
        }
        sprite->under = ' ';
    } else {
        sprite->under = floor;
    }
    sprite->ypos = toY;
    sprite->xpos = toX;
    present(sprite);
}

void attemptMove(THING* sprite, int incrY, int incrX) {
    // take a sprite and a pair of coord modifiers, make a move if possible
    int toY = sprite->ypos+incrY;
    int toX = sprite->xpos+incrX;
    // look at the glyph on the tile
    chtype floor = mvwinch(sprite->room, toY, toX);
    if (floor == '@' || isalpha(floor)) {
        // if it is the player or a monster, fight
        combat(sprite, toY, toX);
    } else if (floor == '#') {
        // a wall
    } else if (floor == ' ' || ispunct(floor)) {
        // if it is blank or an object, move
        stepSprite(sprite, toY, toX);
    }
}

int player_act(THING* sprite) {
    // take a thing (which is the player), return a command key
    chtype ch;
    if (sprite != player) {
        mvaddstr(1, 0, "wrong sprite, expected player");
        refresh(); 
        getch();
        exit(1);
    }
    // return immediately (with a quit command) if player is dead
    if (player->isDead)
        return 'Q';
    // get player (user) command
    ch = wgetch(player->room);
    if (player->isConfused) {
        ch = 49 + rnd(8);
    }
    if (player->isAsleep) {
        ch = '5';
    }
    // clear the indices array for selecting inventory objects
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        allowedIndices[i] = false;
    }
    // handle some extended key codes
    switch (ch) {
        case 258: ch = '2'; break; //down
        case 259: ch = '8'; break; //up
        case 260: ch = '4'; break; //left
        case 261: ch = '5'; break; //right
        case 265: ch = 'h'; break; //F1
    }
    // call a handler for the selected action
    actionHandlers[ch](player);
    return ch;
}

static chtype seekPlayer(THING* sprite) {
    // take a sprite, calculate distance and direction to player, return a command to close distance if close enough
    chtype ch;
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
        // player is too far away, just wander
        ch = 49 + rnd(8);
    }
    return ch;
}

int sprite_act(THING* sprite) {
    // take a sprite, which is not the player, return a command key
    int ch;
    if (sprite == player) {
        mvaddstr(1, 0, "wrong sprite, did not expect player");
        refresh(); 
        getch();
        exit(1);
    }
    // return immediately (with a null command) if sprite is dead
    if (sprite->isDead)
        return 0;
    if (sprite->isImmobile || sprite->isAsleep) {
        ch = '5';
    } else if (sprite->isAggressive) {
        ch = seekPlayer(sprite);
    } else {
        // not interested in seeking out player
        ch = 49 + rnd(8);
    }
    // call a handler for the selected (movement) action
    actionHandlers[ch](sprite);
    return ch;
}

WINDOW* newPopup(int lines) {
    // take a number of lines, return a new window
    return create_newwin(lines, 30, 2, 72);
}

chtype endPopup(WINDOW* win) {
    // take a window, return a character code from it and destroy the window
    wrefresh(win);
    chtype ch = wgetch(win);
    werase(win);
    destroy_win(win);
    return ch;
}

void msg(const char *msg) {
    mvprintw(1, 0, "%s", msg);
    clrtoeol();
    refresh();
}

void dumpInventory(int i) {
    // take an inventory number, remove that item
    inventory[i]->inInventory = false;
    for (int j = i + 1; j < inventoryFill; i++, j++) {
        inventory[i] = inventory[j];
    }
    inventoryFill--;
}

void combat(THING* sprite, int atY, int atX) {
    // take a sprite and a pair of coords, resolve one round of combat there
    THING* other = locateSprite(atY, atX);
    if (sprite == player || other == player)
        // note that the player is in combat and not healing
        player->isInCombat = true;
    int combatRoll = rnd(20) + 1;
    if (combatRoll+sprite->wplus >= (21-sprite->stats->level)-other->armour) {
        msg("hit!");
        int damage = dice2(sprite->damage);
        other->stats->currHp -= damage;
        if (other == player) {
            player->isInjured = true;
        }
        if (other->stats->currHp <= 0) {
            // other is killed
            if ('A' <= other->glyph && other->glyph <= 'Z') {
                mvprintw(1, 0, "The %s is killed!", monsterNames[other->glyph-'A']);
            } else {
                mvprintw(1, 0, "The other is killed!");
            }
            other->isDead = true;
            other->glyph = '%';
            present(other);
            sprite->stats->exp += other->expAward;
            if (sprite == player && player->stats->exp >= expForLevel(player->stats->level + 1)) {
                player->stats->level++;
                player->stats->fullHp += hpIncrForLevel(player->stats->level);
            }
        }
        //TODO could attempt to escape instead
        other->isAggressive = true;
    } else {
        msg("miss!");
    }
}

THING* addGold() {
    // place some gold and return it
    THING* t = present(place(newThing(T_Item, '$')));
    t->gold = 2 + rnd(14);
    return t;
}

void getDir() {
    // set globals deltaY and deltaX either by input or by chance, if confused
    const char* prompt;
    msg(prompt = "Which direction?");
    bool done = false;
    chtype ch;
    while (!done) {
        ch = wgetch(map);
        switch (ch) {
            case '1': deltaY =  1; deltaX = -1; done = true; break;
            case '2': deltaY =  1; deltaX =  0; done = true; break;
            case '3': deltaY =  1; deltaX = +1; done = true; break;
            case '4': deltaY =  0; deltaX = -1; done = true; break;
            case '6': deltaY =  0; deltaX = +1; done = true; break;
            case '7': deltaY = -1; deltaX = -1; done = true; break;
            case '8': deltaY = -1; deltaX =  0; done = true; break;
            case '9': deltaY = -1; deltaX = +1; done = true; break;
        }
        if (!done)
            msg(prompt);
    }
    if (player->isConfused && rnd(100) + 1 > 80) {
        do {
            deltaY = dice(1, 3) - 1;
            deltaX = dice(1, 3) - 1;
        } while (deltaY == 0 && deltaX == 0);
    }
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void destroy_win(WINDOW *local_win) {	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

