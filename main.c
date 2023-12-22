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
        case 2: return 3 + random() % (22-15);
        case 3: return 3 + random() % (29-18);
        case 4: return 8 + random() % (37-26);
        case 5: return 8 + random() % (44-34);
        case 6: return 7 + random() % (50-41);
        case 7: return 15 + random() % (57-56); // weird
        case 8: return 6 + random() % (66-62);
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

int dlevel = 1;

int main() {	
	initscr();
	raw();
	noecho();

    intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

    refresh();

    srandom(time(NULL));

    WINDOW* map = create_newwin(37, 72, 2, 0);
    keypad(map, TRUE);

    //genMap(35, 90, 35, 5, 1, 10);
    genMap9(35, 70);

    //showMap(map);
    showMap9(map);
    wrefresh(map);
            
    curs_set(0);
    int y, x;
    for (int n = 0; n < random() % 5 + 1; n++) {
        addGold(map);
    }
    addRing(map);
    addWeapon(map);
    addScroll(map);
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
        // status line
        mvprintw(LINES-1, 0, "Level: %d  Gold: %-5d  HP: %d(%d)  Str: %-2d  AC: %-2d  Level/Exp: %d/%ld",
                dlevel, player->gold, player->stats->currHp, player->stats->fullHp, 
                player->stats->currStrength, worn==NULL?0:player->armour, player->stats->level,
                player->stats->exp); clrtoeol(); refresh();

        if (player->isLevitating) {
            player->levitationDuration--;
        }
        if (player->isHasted) {
            player_act(map, player);
            player->hasteDuration--;
        }
        if (player->isConfused) {
            player->confusionDuration--;
        }
        if (player->isAsleep) {
            player->sleepDuration--;
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
        ch = player_act(map, player);
        for (THING* thing = things; ch != 'Q' && thing != NULL; thing = thing->next) {
            if (thing->type == T_Sprite && thing != player) {
                sprite_act(map, thing);
            }
        }

        if (player->isHasted && player->hasteDuration <= 0) {
            player->isHasted = false;
        }
        if (player->isLevitating && player->levitationDuration <= 0) {
            player->isLevitating = false;
            mvaddstr(1, 0, "your feet touch the ground again"); clrtoeol();
        }
        if (player->isConfused && player->confusionDuration <= 0) {
            player->isConfused = false;
            mvaddstr(1, 0, "you feel less confused now"); clrtoeol();
        }
        if (player->isAsleep && player->sleepDuration <= 0) {
            player->isAsleep = false;
            mvaddstr(1, 0, "you wake up"); clrtoeol();
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

    destroy_win(map);
	endwin();

	return 0;
}

int player_act(WINDOW* room, THING* sprite) {
    // take a window and a thing (which is the player), return a command key
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
    ch = wgetch(room);
    if (player->isConfused) {
        ch = 49 + random() % 8;
    }
    if (player->isAsleep) {
        ch = '5';
    }
    // clear the indices array for selecting inventory objects
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        allowedIndices[i] = false;
    }
    // deal with some extended key codes
    switch (ch) {
        case 258: ch = '2'; break; //down
        case 259: ch = '8'; break; //up
        case 260: ch = '4'; break; //left
        case 261: ch = '5'; break; //right
        case 265: ch = 'h'; break; //F1
    }
    // call a handler for the selected action
    actionHandlers[ch](room, player);
    return ch;
}

chtype seekPlayer(THING* sprite) {
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
        // player too far away, just wander
        ch = 49 + random() % 8;
    }
    return ch;
}

int sprite_act(WINDOW* room, THING* sprite) {
    // take a window and a sprite, which is not the player, return a command key
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
    if (sprite->isAggressive) {
        ch = seekPlayer(sprite);
    } else {
        // not interested in seeking out player
        ch = 49 + random() % 8;
    }
    // call a handler for the selected (movement) action
    actionHandlers[ch](room, sprite);
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

const char* wandNames[] = {
    "wand of light",
    "wand of striking",
    "wand of lightning",
    "wand of fire",	
    "wand of cold",
    "wand of polymorph",
    "wand of magic missile",
    "wand of haste monster",
    "wand of slow monster",
    "wand of drain life",
    "wand of nothing",	
    "wand of teleport away",
    "wand of teleport to",
    "wand of cancellation",
};

const char* staffNames[] = {
    "staff of light",
    "staff of striking",
    "staff of lightning",
    "staff of fire",	
    "staff of cold",
    "staff of polymorph",
    "staff of magic missile",
    "staff of haste monster",
    "staff of slow monster",
    "staff of drain life",
    "staff of nothing",	
    "staff of teleport away",
    "staff of teleport to",
    "staff of cancellation",
};

void zapEffect(int i) {
    // take an inventory number, check typeId for that stick and handle effect
    THING* t = inventory[i];
    switch (t->typeId) {
        case WS_LIGHT:
            // Has 10-19 charges. Illuminates the room.
            break;
        case WS_HIT:
            // W/S of Striking, which normally does 2d8 + 4 damage, or it has a 5% chance of doing 3d8 + 9 damage.
            break;
        case WS_ELECT:
            // Inflicts 6d6 damage for up to 6 tiles. Bounces off walls.
            break;
        case WS_FIRE:
            // Inflicts 6d6 damage for up to 6 tiles. Bounces off walls. Dragons are immune.
            break;
        case WS_COLD:
            // Inflicts 6d6 damage for up to 6 tiles. Bounces off walls. Yetis are immune.
            break;
        case WS_POLYMORPH:
            // Changes a monster type.
            break;
        case WS_MISSILE:
            // Inflicts 1d4 damage on a single target.
            break;
        case WS_HASTE_M:
            // Hastens a monster.
            break;
        case WS_SLOW_M:
            // Slows a monster.
            break;
        case WS_DRAIN:
            // Drains half of the hero's hp, then removes the same amount of health evenly from visible monsters.
            break;
        case WS_NOP:
            // Doesn't do anything.
            break;
        case WS_TELAWAY:
            // Teleports a monster randomly on the map.
            break;
        case WS_TELTO:
            // Causes the monster to teleport next to the player.
            break;
        case WS_CANCEL:
            // Suppresses monster's special abilities.
            break;
    }
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

void attemptMove(WINDOW* room, THING* sprite, int incrY, int incrX) {
    // take a window, a sprite, and a pair of coord modifiers, move if possible
    int toY = sprite->ypos+incrY;
    int toX = sprite->xpos+incrX;
    // look at the glyph on the tile
    chtype floor = mvwinch(room, toY, toX);
    if (floor == '@' || isalpha(floor)) {
        // if it is the player or a monster, fight
        combat(sprite, toY, toX);
    } else if (floor == '#') {
        // a wall
    } else if (floor == ' ' || ispunct(floor)) {
        // if it is blank or an object, move
        stepSprite(room, sprite, floor, toY, toX);
    }
}

void stepSprite(WINDOW* room, THING* sprite, chtype floor, int toY, int toX) {
    // take a window, a sprite, a floor glyph, and a pair of coords, and move there
    mvwaddch(room, sprite->ypos, sprite->xpos, sprite->under);
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

int dice(int n, int s) {
    // take a number n of dice of size s, return a dice roll
    // n=number of dice
    // s=size of the die
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += random() % s + 1;
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

void combat(THING* sprite, int atY, int atX) {
    // take a sprite and a pair of coords, resolve one round of combat there
    THING* other = locateSprite(atY, atX);
    if (sprite == player || other == player)
        // note that the player is in combat and not healing
        player->isInCombat = true;
    int combatRoll = random() % 20 + 1;
    if (combatRoll+sprite->wplus >= (21-sprite->stats->level)-other->armour) {
        mvaddstr(1, 0, "hit!");
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
        mvaddstr(1, 0, "miss!");
    }
    clrtoeol();
}

void getOpenLocation(WINDOW* win, int *y, int *x) {
    // take a window and a pair of coords addresses, fill the coords with an open location
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

/* from init.c of Rogue3.6.3. For inspiration only, delete before release TODO.
#define ___ 1
#define _x {1,1}
struct monster monsters[26] = {
	// Name		 CARRY	FLAG             str, exp, lvl, amr, hpt, dmg
	{ "giant ant",	 0,	 ISMEAN,	    { _x, 10,   2,   3, ___, "1d6" } },
	{ "bat",	     0,	 0,	            { _x,  1,   1,   3, ___, "1d2" } },
	{ "centaur",	 15, 0,	            { _x, 15,   4,   4, ___, "1d6/1d6" } },
	{ "dragon",	     100,ISGREED,       { _x,9000, 10,  -1, ___, "1d8/1d8/3d10" } },
	{ "floating eye",0,	 0,	            { _x,  5,   1,   9, ___, "0d0" } },
	{ "violet fungi",0,	 ISMEAN,	    { _x, 85,   8,   3, ___, "000d0" } },
	{ "gnome",	     10, 0,	            { _x,  8,   1,   5, ___, "1d6" } },
	{ "hobgoblin",	 0,	 ISMEAN,	    { _x,  3,   1,   5, ___, "1d8" } },
	{ "inv. stalker",0,  ISINVIS,       { _x,120,   8,   3, ___, "4d4" } },
	{ "jackal",	     0,	 ISMEAN,	    { _x,  2,   1,   7, ___, "1d2" } },
	{ "kobold",	     0,	 ISMEAN,	    { _x,  1,   1,   7, ___, "1d4" } },
	{ "leprechaun",	 0,	 0,	            { _x, 10,   3,   8, ___, "1d1" } },
	{ "mimic",	     30, 0,	            { _x,140,   7,   7, ___, "3d4" } },
	{ "nymph",	     100,0,	            { _x, 40,   3,   9, ___, "0d0" } },
	{ "orc",	     15, ISBLOCK,       { _x,  5,   1,   6, ___, "1d8" } },
	{ "purple worm", 70, 0,	            { _x,7000, 15,   6, ___, "2d12/2d4" } },
	{ "quasit",	     30, ISMEAN,	    { _x, 35,   3,   2, ___, "1d2/1d2/1d4" } },
	{ "rust monster",0,	 ISMEAN,	    { _x, 25,   5,   2, ___, "0d0/0d0" } },
	{ "snake",	     0,	 ISMEAN,	    { _x,  3,   1,   5, ___, "1d3" } },
	{ "troll",	     50, ISREGEN|ISMEAN,{ _x, 55,   6,   4, ___, "1d8/1d8/2d6" } },
	{ "umber hulk",	 40, ISMEAN,	    { _x,130,   8,   2, ___, "3d4/3d4/2d5" } },
	{ "vampire",	 20, ISREGEN|ISMEAN,{ _x,380,   8,   1, ___, "1d10" } },
	{ "wraith",	     0,	 0,	            { _x, 55,   5,   4, ___, "1d6" } },
	{ "xorn",	     0,	 ISMEAN,	    { _x,120,   7,  -2, ___, "1d3/1d3/1d3/4d6" } },
	{ "yeti",	     30, 0,	            { _x, 50,   4,   6, ___, "1d6/1d6" } },
	{ "zombie",	     0,	 ISMEAN,	    { _x,  7,   2,   8, ___, "1d8" } }
};
#undef ___
 */

THING* addWand(WINDOW* win) {
    // take a window, create a wand and return it
    int i = random() % NSTICKS;
    const char *descrs[] = {
        "aluminium wand",
        "brass wand",
        "bronze wand",
        "copper wand",
        "gold wand",
        "iron wand",
        "lead wand",
        "nickel wand",
        "pewter wand",
        "silver wand",
        "steel wand",
        "tin wand",
        "titanium wand",
        "zinc wand",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '/', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    t->typeId = i;
    return t;
}

THING* addStaff(WINDOW* win) {
    // take a window, create a staff and return it
    int i = random() % NSTICKS;
    const char *descrs[] = {
        "banyan staff",
        "birch staff",
        "cedar staff",
        "cherry staff",
        "driftwood staff",
        "ebony staff",
        "elm staff",
        "eucalyptus staff",
        "ironwood staff",
        "mahogany staff",
        "maple staff",
        "oak staff",
        "plain staff",
        "redwood staff",
        "teak staff",
        "walnut staff",
    };
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '/', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    t->typeId = i;
    return t;
}

THING* addGold(WINDOW* win) {
    // take a window, place some gold and return it
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '$', y, x);
    t->gold = 2 + random() % 14;
    return t;
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

