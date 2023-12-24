#include "main.h"

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

#define NMETAL (14)
const char* metal[NMETAL] = {
    "aluminium",
    "brass",
    "bronze",
    "copper",
    "gold",
    "iron",
    "lead",
    "nickel",
    "pewter",
    "silver",
    "steel",
    "tin",
    "titanium",
    "zinc"
};

#define NWOOD (16)
const char* wood[NWOOD] = {
    "banyan",
    "birch",
    "cedar",
    "cherry",
    "driftwood",
    "ebony",
    "elm",
    "eucalyptus",
    "ironwood",
    "mahogany",
    "maple",
    "oak",
    "plain",
    "redwood",
    "teak",
    "walnut",
};

const char* ws_made[NSTICKS];
const char* ws_type[NSTICKS];

int wsprobs[NSTICKS] = {
    12, 9, 3, 3, 3, 15, 10, 9, 11, 9, 1, 5, 5, 5
};
int wspt[NSTICKS];

THING* addStick() {
    // create a wand and return it
    int i = pickOne(wspt, NSTICKS);
    THING* t;
    if (strcmp("wand", ws_type[i]) == 0) {
        t = newThing(T_Item, '/');
        snprintf(t->descr, sizeof t->descr, "%s %s", ws_made[i], "wand");
    } else {
        t = newThing(T_Item, '\\');
        snprintf(t->descr, sizeof t->descr, "%s %s", ws_made[i], "staff");
        t->ncharges = rnd(5) + 3;
    }
    t->isEquippable = true;
    t->typeId = i;
    return t;
}

void initSticks() {
    // set up sticks probability table
    wspt[0] = wsprobs[0];
    for (int i = 1; i < NSTICKS; i++) {
        wspt[i] = wsprobs[i] + wspt[i-1];
    }
    //TODO check total = 100
    int i, j;
    bool woodused[NWOOD];
    bool metused[NMETAL];
    for (i = 0; i < NWOOD; i++)
        woodused[i] = false;
    for (i = 0; i < NMETAL; i++)
        metused[i] = false;
    for (i = 0; i < NSTICKS; i++) {
        for (;;)
            if (rnd(2)) {
                j = rnd(NMETAL);
                if (!metused[j]) {
                    metused[j] = true;
                    ws_made[i] = metal[j];
                    ws_type[i] = "wand";
                    break;
                }
            } else {
                j = rnd(NWOOD);
                if (!woodused[j]) {
                    woodused[j] = true;
                    ws_made[i] = wood[j];
                    ws_type[i] = "staff";
                    break;
                }
            }
        //ws_know[i] = FALSE;
        //ws_guess[i] = NULL;
    }
}

void zapEffect(int i, bool hasDir) {
    int x, y;
    // take an inventory number, check typeId for that stick and handle effect
    THING* t = inventory[i];
    if (t->glyph != '/' && t->glyph != '\\') {
        msg("You can't zap with that");
        return;
    }
    if (t->ncharges == 0) {
        msg("Nothing happens");
        return; 
    }
    if (!hasDir) {
        do {
            deltaY = rnd(3) - 1;
            deltaX = rnd(3) - 1;
        } while (deltaY == 0 && deltaX == 0);
    }
    switch (t->typeId) {
        case WS_LIGHT:
            // Has 10-19 charges. Illuminates the room.
            break;
        case WS_HIT:
            // W/S of Striking, which normally does 2d8 + 4 damage, or it has a 5% chance of doing 3d8 + 9 damage.
            y = player->ypos + deltaY;
            x = player->xpos + deltaX;
            THING* other = locateSprite(y, x);
            if (other != NULL) {
                if (rnd(100) + 1 <= 95) {
                    other->stats->currHp -= (dice2("2d8") + 4);
                } else {
                    other->stats->currHp -= (dice2("3d8") + 9);
                }
            }
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
        case WS_TELAWAY:
            // Teleports a monster randomly on the map.
        case WS_TELTO:
            // Causes the monster to teleport next to the player.
        case WS_CANCEL:
            // Suppresses monster's special abilities.
            y = player->ypos;
            x = player->xpos;
            chtype ch;
            while (isFree(ch = mvwinch(player->room, y, x))) {
                y += deltaY;
                x += deltaX;
            }
            if (isupper(ch)) {
                THING* other = locateSprite(x, y);
                if (ch == 'F') {
                    // TODO player is held
                }
                if (t->typeId == WS_POLYMORPH) {
                    freeObject(other);
                    deltaY = y;
                    deltaX = x;
                    placeAt(addMonster(rnd(26) + 'A'), deltaY, deltaX);
                    // TODO player knows that it's a W/S of Polymorph unless new kind = old
                } else if (t->typeId == WS_CANCEL) {
                    other->isCanceled = true;
                    other->isInvisible = false;
                } else if (t->typeId == WS_TELAWAY) {
                    mvwaddch(other->room, other->ypos, other->xpos, other->under);
                    place(other);
                } else if (t->typeId == WS_TELTO) {
                    mvwaddch(other->room, other->ypos, other->xpos, other->under);
                    other->ypos = player->ypos + deltaY;
                    other->xpos = player->xpos + deltaX;
                    other->under = ' ';
                    present(other);
                }
            }
            break;
        case WS_MISSILE:
            // Inflicts 1d4 damage on a single target.
            y = player->ypos + deltaY;
            x = player->xpos + deltaX;
            THING* z = newThing(T_Item, '*');
            z->ypos = y;
            z->xpos = x;
            for (;;) {
                if (!isFree(z->under)) {
                    if (isupper(z->under)) {
                        //TODO other gets to save vs magic
                        THING* other = locateSprite(y, x);
                        other->stats->currHp -= dice2("1d4");
                        break;
                    }
                } else {
                    present(z);
                }
                mvwaddch(z->room, y, x, z->under);
                y += deltaY;
                x += deltaX;
                z->ypos = y;
                z->xpos = x;
            }
            // TODO w/s is identified as magic missile
            break;
        case WS_HASTE_M:
            // Hastens a monster.
        case WS_SLOW_M:
            // Slows a monster.
            y = player->ypos + deltaY;
            x = player->xpos + deltaX;
            chtype what = mvwinch(player->room, y, x);
            while (isFree(what)) {
                y += deltaY;
                x += deltaX;
                what = mvwinch(player->room, y, x);
            }
            if (isupper(what)) {
                THING* other = locateSprite(y, x);
                if (t->typeId == WS_HASTE_M) {
                    if (other->isSlowed)
                        other->isSlowed = false;
                    else
                        other->isHasted = true;
                } else {
                    if (other->isHasted)
                        other->isHasted = false;
                    else
                        other->isSlowed = true;
                }
            }
            break;
        case WS_DRAIN:
            // Drains half of the hero's hp, then removes the same amount of health evenly from visible monsters.
            break;
        case WS_NOP:
            // Doesn't do anything.
            break;
    }
}

