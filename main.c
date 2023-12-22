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

THING* locateThing(int ypos, int xpos) {
    // take y, x coordinates, traverse thing list and return the first to occupy that place
    for (THING* thing = things; thing != NULL; thing = thing->next) {
        if (ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        }
    }
    return NULL;
}

THING* locateObject(int ypos, int xpos) {
    // take y, x coordinates, traverse thing list and return the first object to occupy that place
    for (THING* thing = things; thing != NULL; thing = thing->next) {
        if ((thing->type == T_Item || thing->type == T_Structure)
                && ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        }
    }
    return NULL;
}

THING* locateSprite(int ypos, int xpos) {
    // take y, x coordinates, traverse thing list and return the first sprite to occupy that place
    for (THING* thing = things; thing != NULL; thing = thing->next) {
        if (thing->type == T_Sprite && ypos == thing->ypos && xpos == thing->xpos) {
            return thing;
        }
    }
    return NULL;
}

/* From weapons.c of Rogue3.6.3. For inspiration only, delete before release TODO.
static struct init_weps {
    char *iw_dam;
    char *iw_hrl;
    char iw_launch;
    int iw_flags;
} init_dam[MAXWEAPONS] = {
    "2d4", "1d3", NONE, 0,		           // Mace
    "1d10", "1d2", NONE,0,		           // Long sword
    "1d1", "1d1", NONE,	0,		           // Bow
    "1d1", "1d6", BOW,	ISMANY|ISMISL,	   // Arrow
    "1d6", "1d4", NONE,	ISMISL,		       // Dagger
    "1d2", "1d4", SLING,ISMANY|ISMISL,	   // Rock
    "3d6", "1d2", NONE,	0,		           // 2h sword
    "0d0", "0d0", NONE, 0,		           // Sling
    "1d1", "1d3", NONE,	ISMANY|ISMISL,	   // Dart
    "1d1", "1d1", NONE, 0,		           // Crossbow
    "1d2", "1d10", CROSSBOW, ISMANY|ISMISL,// Crossbow bolt
    "1d8", "1d6", NONE, ISMISL,		       // Spear
};
 */

const char* weaponNames[] = {
    "mace",
    "long sword",
    "dagger",
    "two-handed sword",
    "spear",
};

const char* weaponDamage[] = {
    "2d4",
    "3d4",
    "1d6",
    "4d4",
    "2d3",
};

void wieldEffect(int i) {
    // take an inventory number, set wielded to that weapon
    //TODO
    THING* t = inventory[i];
    wielded = t;
    player->damage = wielded->damage;
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

static void msg(const char *msg) {
    mvprintw(1, 0, "%s", msg);
    clrtoeol();
    refresh();
}

const char* scrollNames[] = {
    "scroll of confuse monster",
    "scroll of magic mapping",
    "scroll of light",
    "scroll of hold monster",
    "scroll of sleep",
    "scroll of enchant armour",
    "scroll of identify",
    "scroll of protect armour",
    "scroll of scare monster",
    "scroll of gold detection",
    "scroll of teleportation",
    "scroll of enchant weapon",
    "scroll of create monster",
    "scroll of remove curse",
    "scroll of aggravate monster",
    "scroll of blank paper"
};

void readEffect(int i) {
    // take an inventory number, check typeId for that scroll and handle effect
    THING* t = inventory[i];
    msg("As you read the scroll, it vanishes.");
    switch (t->typeId) {
        case S_CONFUSE_M:
            // When you strike enemies using a melee weapon there is a chance that they will become confused.
            msg("Your hands begin to glow red");
            player->canConfuse = true;
            //TODO expire?
            break;
        case S_MAGIC_MAP:
            // Its affect reads: "this scroll seems to have a map on it". It reveals all the rooms, corridors, traps, and stairs down on that dungeon level. It does not show enemies or items.
            msg("Oh, now this scroll has a map on it.");
            break;
        case S_LIGHT:
            break;
        case S_HOLD_MONS:
            // Its affect reads: "the monster freezes". It keeps monsters from moving, obviously.
            break;
        case S_SLEEP:
            // Its affect reads: "you fall asleep". Obviously, you fall asleep for some amount of time (similar to the sleeping gas trap).
            player->isAsleep = true;
            mvaddstr(1, 0, "you fall asleep"); clrtoeol();
            player->sleepDuration = 50;
            break;
        case S_ENCH_ARM:
            // Its affect reads: "your armor glows blue for a moment". It increases the bonus on your armor by 1. If your armor was cursed (e.g. -3), it will no longer be so and the penalty for wearing it will be one less (e.g. -2).
            if (worn != NULL) {
                msg("Your armour glows faintly for a moment");
                //TODO reduce ac
                //TODO remove curse
            }
            break;
        case S_IDENTIFY:
            // It identifies another item of your choosing when used.
            msg("This scroll is an identify scroll");
            //TODO identify
            break;
        case S_PROT_ARM:
            // Its affect reads: "your armor is covered by a shimmering gold shield".
            break;
        case S_SCARE_M:
            msg("You hear maniacal laughter in the distance.");
            break;
        case S_GOLD_DET:
            break;
        case S_TELEPORT:
            // It teleports you to a random location in the current dungeon level.
            mvwaddch(player->room, player->ypos, player->xpos, player->under);
            int y, x;
            getOpenLocation(player->room, &y, &x);
            player->ypos = y;
            player->xpos = x;
            player->under = ' ';
            present(player);
            break;
        case S_ENCH_WPN:
            // Its affect reads: "your weapon glows blue for a moment". It increases the bonus on your weapon by +0,+1. If your weapon was cursed, it will no longer be so and the penalty for weilding it will be one less.
            // effect on wielded weapon only
            if (wielded == NULL) {
                msg("You feel a strange sense of loss.");
            } else {
                mvaddstr(1, 0, "your weapon glows blue for a moment");
                wielded->wplus += random() % 2;
                // TODO cursed status
            }
            break;
        case S_CREATE_M:
            // Just like its name says, it creates a random monster (suitable for that dungeon level) adjacent to you. This monster will be hostile.
            // TODO make it adjacent
            addMonster(t->room);
            break;
        case S_REM_CURS:
            if (worn != NULL)
                worn->isCursed = false;
            if (wielded != NULL)
                wielded->isCursed = false;
            if (left != NULL)
                left->isCursed = false;
            if (right != NULL)
                right->isCursed = false;
            msg("You feel as if somebody is watching over you.");
            break;
        case S_AGGR_MON:
            // Monsters that may have been docile are now hostile towards you.
            msg("You hear a high pitched humming noise.");
            //TODO aggravate
            break;
        case S_BLNK_PAP:
            msg("This scroll seems to be blank.");
            break;
    }
}

const char *ringNames[] = {
    "ring of adornment",
    "ring of dexterity",
    "ring of maintain armor",
    "ring of see invisible",
    "ring of teleportation",
};

void equipEffect(int i) {
    // take an inventory number, check typeId for that ring and handle effect
    // TODO Rings may be worn on either the left or right hand with no change in effect. While worn, however, they will make you hungrier quicker.
    THING* t = inventory[i];
    switch (t->typeId) {
        case R_ADORN:
            // no special effect
            break;
        case R_DEXT:
            // When put on, your dexterity is adjusted by the ring's stated modifier.
            break;
        case R_MNT_ARM:
            // When put on, your armor cannot be damaged by traps or enemies (such as the aquator).
            break;
        case R_SEE_INVIS:
            // When put on, you will be able to see invisible enemies.
            break;
        case R_TELEPORT:
            // When put on, you will be randomly teleported to another location in the current dungeon level every few actions. This is a cursed ring and cannot be removed until uncursed.
            break;
    }
}

const char *armourNames[] = {
    "none",
    "leather armour",
    "ring mail",
    "scale mail",
    "chain mail",
    "splint mail",
    "banded mail",
    "plate mail",
};

int acValue[] = {
    10,
    8,
    7,
    6,
    5,
    4,
    4,
    3
};

void wearEffect(int i) {
    // take an inventory number, set worn to that armour, copy armour value to player
    THING* t = inventory[i];
    worn = t;
    player->armour = worn->armour;
    // TODO lesser or greater quality variants
}

const char *potionNames[] = {
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

void drinkEffect(int i) {
    // take an inventory number, check typeId for that potion and handle effect
    THING* t = inventory[i];
    //TODO replace description
    //TODO timer for (some) effects
    switch (t->typeId) {
        case P_BLIND:
            // Its effect reads: "a cloak of darkness falls around you". You are unable to see enemies, items, and the dungeon around you.
            // TODO
            break;
        case P_CONFUS:
            // For a short amount of time, you are unable to move in the direction you press. 
            msg("you feel confused");
            player->isConfused = true;
            player->confusionDuration = 50;
            break;
        case P_DET_MON:
            // When read [sic] no text appears. However, it reveals all monsters on the current dungeon level.
            // TODO
            break;
        case P_DET_THG:
            // When read no text appears. However, it reveals all items and gold present in the current dungeon level.
            // TODO
            break;
        case P_EXT_HEAL:
            msg("you begin to feel much better");
            player->stats->currHp += 27;
            if (player->stats->currHp > player->stats->fullHp)
                player->stats->currHp = player->stats->fullHp;
            break;
        case P_HALLUC:
            // Its effect reads: "oh wow, everything seems so cosmic". When quaffed all enemies and items will change symbol from turn to turn. When its effect wears off it reads: "everything looks SO boring now".
            // TODO
            break;
        case P_HASTE:
            //  It may appear as though you are moving at your normal speed but you can sometimes move twice before a monster moves once.
            mvaddstr(1, 0, "you feel yourself moving much faster");
            player->hasteDuration = 50;
            break;
        case P_HEALING:
            // Its effect reads: "you begin to feel better". It restores a number of Hp (13).
            mvaddstr(1, 0, "you begin to feel better"); clrtoeol();
            player->stats->currHp += 13;
            if (player->stats->currHp > player->stats->fullHp)
                player->stats->currHp = player->stats->fullHp;
            break;
        case P_INC_STR:
            // When quaffed your current strength increases by one (like a potion of restore strength), however, if you strength is at max, your maximum strength increases by one.
            mvaddstr(1, 0, "you feel stronger now, what bulging muscles!");
            player->stats->currStrength++;
            if (player->stats->currStrength > player->stats->fullStrength)
                player->stats->fullStrength++;
            break;
        case P_LEVIT:
            // While levitating you cannot fall into traps or pickup items.
            mvaddstr(1, 0, "you start to float in the air");
            player->isLevitating = true;
            player->levitationDuration = 50;
            break;
        case P_POISON:
            // It reduces your current strength by 3.
            mvaddstr(1, 0, "you feel very sick now");
            player->stats->currStrength -= 3;
            break;
        case P_RS_LVL:
            //  You instantly attain the next level (Exp automatically adjusted).
            mvaddstr(1, 0, "you suddenly feel much more skillful");
            player->stats->level++;
            player->stats->exp = expForLevel(player->stats->level);
            player->stats->fullHp += hpIncrForLevel(player->stats->level);
            break;
        case P_REST_STR:
            // Your current strength increases by one up to your maximum strength.
            mvaddstr(1, 0, "this tastes great, you feel warm all over");
            player->stats->currStrength++;
            if (player->stats->currStrength > player->stats->fullStrength)
                player->stats->currStrength--;
            break;
        case P_SEE_INVIS:
            // Its effect reads: "hmm, this potion tastes like slime-mold juice". Presumably it lets you see invisible things...
            // TODO
            break;
    }
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

static void freeObject(THING* o) {
    // take a thing, remove it from the list and free it
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

static int dice(int n, int s) {
    // take a number n of dice of size s, return a dice roll
    // n=number of dice
    // s=size of the die
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += random() % s + 1;
    }
    return total;
}

static int dice2(const char* code) {
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

void present(THING* sprite) {
    // take a sprite, show its glyph
    mvwaddch(sprite->room, sprite->ypos, sprite->xpos, sprite->glyph);
    wrefresh(sprite->room);
}

static void getOpenLocation(WINDOW* win, int *y, int *x) {
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

const char* monsterNames[26] = {
    "giant ant",    // A
    "bat",          // B
    "centaur",      // C
    "dragon",       // D
    "floating eye", // E
    "violet fungi", // F
    "goblin",       // G
    "hobgoblin",    // H
    "ice monster",  // I
    "jackal",       // J
    "kobold",       // K
    "leprechaun",   // L
    "mimic",        // M
    "nymph",        // N
    "orc",          // O
    "purple worm",  // P
    "quasit",       // Q
    "rust monster", // R
    "snake",        // S
    "troll",        // T
    "umber hulk",   // U
    "vampire",      // V
    "wraith",       // W
    "xorn",         // X
    "yeti",         // Y
    "zombie"        // Z
};

THING* addMonster(WINDOW* win) {
    // take a window, create a monster and return it
    int y, x, i;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Sprite, 'X', y, x);
    const char *aggr;
    if (dlevel == 1) 
        i = random() % 6;
    else if (dlevel == 2) 
        i = random() % 7;
    else
        i = 0;
    switch (i) {
        case 0:
            t->descr = "bat";
            t->glyph = 'B';
            aggr = "sometimes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d2";
            t->armour = 3;
            t->expAward = 2;
            break;
        case 1:
            t->descr = "floating eye";
            t->glyph = 'E';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = dice(1, 8);
            t->damage = "0d0";
            //TODO
            t->armour = 9;
            t->expAward = 2;
            t->isImmobile = true;
            break;
        case 2:
            t->descr = "goblin";
            t->glyph = 'G';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d6";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 3:
            t->descr = "hobgoblin";
            t->glyph = 'H';
            aggr = "yes";
            t->attack = 3;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d8";
            t->armour = 5;
            t->expAward = 3;
            break;
        case 4:
            t->descr = "kobold";
            t->glyph = 'K';
            aggr = "yes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d4";
            t->armour = 7;
            t->expAward = 2;
            break;
        case 5:
            t->descr = "snake";
            t->glyph = 'S';
            aggr = "yes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d3";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 6:
            t->descr = "ice monster";
            t->glyph = 'I';
            aggr = "no";
            t->attack = 4;
            t->stats->fullHp = dice(1, 8);
            t->damage = "4d4";
            t->armour = 5;
            t->expAward = 5;
            break;
    }
    t->wplus = 0;
    t->stats->currHp = t->stats->fullHp;
    if (strcmp(aggr, "sometimes") == 0) {
        t->isAggressive = random()%2==1?true:false;
    } else if (strcmp(aggr, "no") == 0) {
        t->isAggressive = false;
    } else if (strcmp(aggr, "yes") == 0) {
        t->isAggressive = true;
    }
    return t;
}

THING* addWeapon(WINDOW* win) {
    int i = random() % NWEAPONS;
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, ')', y, x);
    t->descr = weaponNames[i];
    t->damage = weaponDamage[i];
    t->typeId = i;
    return t;
}

THING* addArmour(WINDOW* win) {
    // take a window, create an armour and return it
    // avoid getting the "none" armour
    int i = random() % (NARMOURS-1) + 1;
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '&', y, x);
    t->descr = armourNames[i];
    t->armour = acValue[i];
    t->typeId = i;
    return t;
}

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

/* From init.c of Rogue3.6.3. For inspiration only, remove before release TODO
struct magic_item s_magic[MAXSCROLLS] = {
    { "monster confusion",	 8, 170 },
    { "magic mapping",		 5, 180 },
    { "light",			10, 100 },
    { "hold monster",		 2, 200 },
    { "sleep",			 5,  50 },
    { "enchant armor",		 8, 130 },
    { "identify",		21, 100 },
    { "scare monster",		 4, 180 },
    { "gold detection",		 4, 110 },
    { "teleportation",		 7, 175 },
    { "enchant weapon",		10, 150 },
    { "create monster",		 5,  75 },
    { "remove curse",		 8, 105 },
    { "aggravate monsters",	 1,  60 },
    { "blank paper",		 1,  50 },
    { "genocide",		 1, 200 },
};*/

THING* addScroll(WINDOW* win) {
    // take a window, create a scroll and return it
    int i = random() % NSCROLLS;
#if 0
    const char *idents[] = {
        "scroll of confuse monster",
        "scroll of magic mapping",
        "scroll of light",
        "scroll of hold monster",
        "scroll of sleep",
        "scroll of enchant armour",
        "scroll of identify",
        "scroll of protect armour",
        "scroll of scare monster",
        "scroll of gold detection",
        "scroll of teleportation",
        "scroll of enchant weapon",
        "scroll of create monster",
        "scroll of remove curse",
        "scroll of aggravate monster",
        "scroll of blank paper",
    };
#endif
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '~', y, x);
    t->descr = "a magical scroll";
    t->typeId = i;
    return t;
}

THING* addRing(WINDOW* win) {
    // take a window, create a ring and return it
    int i = random() % NRINGS;
    const char *descrs[] = {
        "diamond ring",
        "emerald ring",
        "ruby ring",
        "opal ring",
        "sapphire ring",
    };
#if 0
    const char *idents[] = {
        "ring of adornment",
        "ring of dexterity",
        "ring of maintain armor",
        "ring of see invisible",
        "ring of teleportation",
    };
#endif
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, ':', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->typeId = i;
    return t;
}

THING* addPotion(WINDOW* win) {
    // take a window, create a potion and return it
    int i = random() % NPOTIONS;
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
#if 0
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
#endif
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '!', y, x);
    t->descr = descrs[i];
    t->isPotable = true;
    //TODO if dropped after being identified, should be true
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

THING* newThing(WINDOW* win, ThingType type, chtype glyph, int y, int x) {
    // take a window, a thing type, a glyph, and a pair of coords, create a thing and return it
    THING* thing = malloc(sizeof(THING));
    if (thing == NULL) {
        mvaddstr(1, 0, "Out of memory, press any key to exit");
        refresh();
        getch();
        exit(1);
    }
    if (type == T_Sprite) {
        STATS* s = malloc(sizeof(THING));
        if (s == NULL) {
            mvaddstr(1, 0, "Out of memory, press any key to exit");
            refresh();
            getch();
            exit(1);
        }
        thing->stats = s;
        thing->stats->level = 1;
        thing->stats->exp = 0;
        thing->stats->fullHp = thing->stats->currHp = 12;
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
    thing->isAggressive = false;
    thing->isDead = false;
    thing->isConfused = false;
    thing->isAsleep = false;
    thing->isHasted = false;
    thing->isLevitating = false;
    thing->isInCombat = false;
    thing->isCursed = false;
    thing->isInjured = false;
    thing->healingCycle = 0;
    thing->attack = 0;
    thing->descr = NULL;
    thing->ident = NULL;
    thing->typeId = -1;
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
            thing->stats->currStrength = thing->stats->fullStrength = 12;
            break;
    }
    thing->gold = 0;
    thing->inInventory = false;
    thing->damage = "1d4";
    thing->wplus = 0;
    thing->armour = 11;
    thing->next = things;
    things = thing;
    thing->under = ' ';
    present(thing);
    return thing;
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

