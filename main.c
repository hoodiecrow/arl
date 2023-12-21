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

int expForLevel(int i) {
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
        werase(status);
        box(status, 0 , 0);
        mvwprintw(status, 1, 1, "Gold: %9d", player->gold);
        mvwprintw(status, 2, 1, "Hit Points: %d (%d)", player->stats->currHp, player->stats->fullHp);
        mvwprintw(status, 3, 1, "Strength: %3d (%3d)", player->stats->currStrength, player->stats->fullStrength);
        mvwprintw(status, 4, 1, "Armour: %s (%d)", worn==NULL?"none":worn->descr, worn==NULL?0:player->armour);
        mvwprintw(status, 5, 1, "RH: %s", right==NULL?"":right->ident);
        mvwprintw(status, 6, 1, "LH: %s", left==NULL?"":left->ident);
        mvwprintw(status, 7, 1, "Level: %d Exp: %d", player->stats->level, player->stats->exp);
        wrefresh(status);
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
        refresh();
        wrefresh(map);
        wrefresh(status);
    }

    destroy_win(map);
	endwin();

	return 0;
}

int player_act(WINDOW* room, THING* sprite) {
    chtype ch;
    if (sprite->isDead)
        return 'Q';
    ch = wgetch(room);
    if (player->isConfused) {
        ch = 49 + random() % 8;
    }
    if (player->isAsleep) {
        ch = '5';
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

int sprite_act(WINDOW* room, THING* sprite) {
    int ch;
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
    actionHandlers[ch](room, sprite);
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

void wieldEffect(int i) {
    //TODO
    (void)i;
    THING* t = inventory[i];
    mvaddstr(1, 0, t->descr); clrtoeol(); refresh();
    if (strcmp(t->descr, "") == 0) {
        (void)t->modifier;
    } else if (strcmp(t->descr, "") == 0) {
        (void)t->modifier;
    } else {
        t->armour = 0;
    }
    // TODO lesser or greater quality variants
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
    mvaddstr(1, 0, "As you read the scroll, it vanishes."); clrtoeol(); refresh();
    if (strcmp(t->ident, "scroll of aggravate monster") == 0) {
        // Its affect reads: "you hear a high pitched humming noise". Monsters that may have been docile are now hostile towards you.
    } else if (strcmp(t->ident, "scroll of confuse monster") == 0) {
        // Its affect reads: "your hands glow red for a moment". When you strike enemies using a melee weapon there is a chance that they will become confused.
    } else if (strcmp(t->ident, "scroll of create monster") == 0) {
        // Just like its name says, it creates a random monster (suitable for that dungeon level) adjacent to you. This monster will be hostile.
        // TODO make it adjacent
        addMonster(t->room);
    } else if (strcmp(t->ident, "scroll of enchant armour") == 0) {
        // Its affect reads: "your armor glows blue for a moment". It increases the bonus on your armor by 1. If your armor was cursed (e.g. -3), it will no longer be so and the penalty for wearing it will be one less (e.g. -2).
    } else if (strcmp(t->ident, "scroll of enchant weapon") == 0) {
        // Its affect reads: "your weapon glows blue for a moment". It increases the bonus on your weapon by +0,+1. If your weapon was cursed, it will no longer be so and the penalty for weilding it will be one less.
        // effect on wielded weapon only
        if (wielded != NULL) {
            mvaddstr(1, 0, "your weapon glows blue for a moment");
            wielded->modifier += random() % 2;
            // TODO cursed status
        }
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
        player->isAsleep = true;
        mvaddstr(1, 0, "you fall asleep"); clrtoeol();
        player->sleepDuration = 50;
    } else if (strcmp(t->ident, "scroll of teleportation") == 0) {
        // It teleports you to a random location in the current dungeon level.
        mvwaddch(player->room, player->ypos, player->xpos, player->under);
        int y, x;
        getOpenLocation(player->room, &y, &x);
        player->ypos = y;
        player->xpos = x;
        player->under = ' ';
        present(player);

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
        player->armour = t->armour = 6;
    } else if (strcmp(t->descr, "chain mail") == 0) {
        player->armour = t->armour = 5;
    } else if (strcmp(t->descr, "leather armour") == 0) {
        player->armour = t->armour = 2;
    } else if (strcmp(t->descr, "plate mail") == 0) {
        player->armour = t->armour = 7;
    } else if (strcmp(t->descr, "ring mail") == 0) {
        player->armour = t->armour = 3;
    } else if (strcmp(t->descr, "scale mail") == 0) {
        player->armour = t->armour = 4;
    } else if (strcmp(t->descr, "splint mail") == 0) {
        player->armour = t->armour = 6;
    } else {
        player->armour = t->armour = 0;
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
        // TODO
    } else if (strcmp(t->ident, "potion of confusion") == 0) {
        // Its effect reads: "you feel confused". For a short amount of time, you are unable to move in the direction you press. When its effect wears off it reads: "you feel less confused now".
        mvaddstr(1, 0, "you feel confused"); clrtoeol();
        player->isConfused = true;
        player->confusionDuration = 50;
    } else if (strcmp(t->ident, "potion of detect monster") == 0) {
        // When read [sic] no text appears. However, it reveals all monsters on the current dungeon level.
        // TODO
    } else if (strcmp(t->ident, "potion of detect things") == 0) {
        // When read no text appears. However, it reveals all items and gold present in the current dungeon level.
        // TODO
    } else if (strcmp(t->ident, "potion of extra healing") == 0) {
        // Its effect reads: "you begin to feel much better". It restores a very large number of Hp (27).
        mvaddstr(1, 0, "you begin to feel much better"); clrtoeol();
        player->stats->currHp += 27;
        if (player->stats->currHp > player->stats->fullHp)
            player->stats->currHp = player->stats->fullHp;
    } else if (strcmp(t->ident, "potion of hallucination") == 0) {
        // Its effect reads: "oh wow, everything seems so cosmic". When quaffed all enemies and items will change symbol from turn to turn. When its effect wears off it reads: "everything looks SO boring now".
        // TODO
    } else if (strcmp(t->ident, "potion of haste self") == 0) {
        //  Its effect reads: "you feel yourself moving much faster". It may appear as though you are moving at your normal speed but you can sometimes move twice before a monster moves once.
        mvaddstr(1, 0, "you feel yourself moving much faster");
        player->hasteDuration = 50;
    } else if (strcmp(t->ident, "potion of healing") == 0) {
        // Its effect reads: "you begin to feel better". It restores a number of Hp (13).
        mvaddstr(1, 0, "you begin to feel better"); clrtoeol();
        player->stats->currHp += 13;
        if (player->stats->currHp > player->stats->fullHp)
            player->stats->currHp = player->stats->fullHp;
    } else if (strcmp(t->ident, "potion of increase strength") == 0) {
        // Its effect reads: "you feel stronger now, what bulging muscles!". When quaffed your current strength increases by one (like a potion of restore strength), however, if you strength is at max, your maximum strength increases by one.
        mvaddstr(1, 0, "you feel stronger now, what bulging muscles!");
        player->stats->currStrength++;
        if (player->stats->currStrength > player->stats->fullStrength)
            player->stats->fullStrength++;
    } else if (strcmp(t->ident, "potion of levitation") == 0) {
        // Its effect reads: "you start to float in the air". While levitating you cannot fall into traps or pickup items.
        mvaddstr(1, 0, "you start to float in the air");
        player->isLevitating = true;
        player->levitationDuration = 50;
    } else if (strcmp(t->ident, "potion of poison") == 0) {
        // Its effect reads: "you feel very sick now". It reduces your current strength by 3.
        mvaddstr(1, 0, "you feel very sick now");
        player->stats->currStrength -= 3;
    } else if (strcmp(t->ident, "potion of raise level") == 0) {
        //  Its effect reads: "you suddenly feel much more skillful". You instantly attain the next level (Exp automatically adjusted).
        mvaddstr(1, 0, "you suddenly feel much more skillful");
        player->stats->level++;
        player->stats->exp = expForLevel(player->stats->level);
        player->stats->fullHp += hpIncrForLevel(player->stats->level);
        //TODO exp points
    } else if (strcmp(t->ident, "potion of restore strength") == 0) {
        // Its effect reads: "this tastes great, you feel warm all over". Your current strength increases by one up to your maximum strength.
        mvaddstr(1, 0, "this tastes great, you feel warm all over");
        player->stats->currStrength++;
        if (player->stats->currStrength > player->stats->fullStrength)
            player->stats->currStrength--;
    } else if (strcmp(t->ident, "potion of see invisible") == 0) {
        // Its effect reads: "hmm, this potion tastes like slime-mold juice". Presumably it lets you see invisible things...
        // TODO
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
        // TODO exp for gold?
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
    int combatRoll = random() % 20 + 1;
    if (combatRoll+sprite->wplus-other->armour >= (21-sprite->stats->level)) {
        mvaddstr(1, 0, "hit!");
        other->stats->currHp--;
        if (other->stats->currHp <= 0) {
            // other is killed
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
            t->stats->fullHp = 1;
            t->expAward = 2;
            break;
        case 1:
            t->descr = "emu";
            t->glyph = 'E';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = 1;
            t->expAward = 2;
            break;
        case 2:
            t->descr = "goblin";
            t->glyph = 'G';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = 2;
            t->expAward = 2;
            break;
        case 3:
            t->descr = "hobgoblin";
            t->glyph = 'H';
            aggr = "yes";
            t->attack = 3;
            t->stats->fullHp = 3;
            t->expAward = 3;
            break;
        case 4:
            t->descr = "kestrel";
            t->glyph = 'K';
            aggr = "sometimes";
            t->attack = 1;
            t->stats->fullHp = 2;
            t->expAward = 2;
            break;
        case 5:
            t->descr = "snake";
            t->glyph = 'S';
            aggr = "sometimes";
            t->attack = 1;
            t->stats->fullHp = 1;
            t->expAward = 2;
            break;
        case 6:
            t->descr = "ice monster";
            t->glyph = 'I';
            aggr = "no";
            t->attack = 4;
            t->stats->fullHp = 4;
            t->expAward = 5;
            break;
    }
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
    thing->isIdentified = false;
    thing->isAggressive = false;
    thing->isDead = false;
    thing->isConfused = false;
    thing->isAsleep = false;
    thing->isHasted = false;
    thing->isLevitating = false;
    thing->attack = 0;
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
    thing->modifier = 0;
    thing->wplus = 0;
    thing->armour = 0;
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

