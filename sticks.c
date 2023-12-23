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

THING* addWand() {
    // create a wand and return it
    int i = rnd(NSTICKS);
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
    THING* t = newThing(T_Item, '/');
    t->descr = descrs[i];
    t->isEquippable = true;
    t->typeId = i;
    return t;
}

THING* addStaff() {
    // create a staff and return it
    int i = rnd(NSTICKS);
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
    THING* t = newThing(T_Item, '\\');
    t->descr = descrs[i];
    t->isEquippable = true;
    t->ncharges = rnd(5) + 3;
    t->typeId = i;
    return t;
}

void zapEffect(int i) {
    int x, y;
    // take an inventory number, check typeId for that stick and handle effect
    THING* t = inventory[i];
    if (t->glyph != '/') {
        msg("You can't zap with that");
        return;
    }
    if (t->ncharges == 0) {
        msg("Nothing happens");
        return; 
    }
    getDir(t->room);
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
            while ((ch = mvwinch(player->room, y, x)) && ch != '#' && !isupper(ch)) {
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
                    present(place(other));
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
                if (z->under == '#' || isupper(z->under)) {
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
            while (what != '#' && !isupper(what)) {
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

