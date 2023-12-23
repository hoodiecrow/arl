#include "main.h"

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

void freeObject(THING* o) {
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

THING* present(THING* sprite) {
    // take a sprite, show its glyph
    mvwaddch(map, sprite->ypos, sprite->xpos, sprite->glyph);
    wrefresh(map);
    return sprite;
}

THING* place(THING* thing) {
    // take a thing, place it in a random, open location
    int maxy, maxx;
    getmaxyx(map, maxy, maxx);
    int ry = rnd(maxy);
    int rx = rnd(maxx);
    while (!isFree(mvwinch(map, ry, rx))) {
        ry = rnd(maxy);
        rx = rnd(maxx);
    }
    thing->ypos = ry;
    thing->xpos = rx;
    thing->under = mvwinch(map, ry, rx);
    mvwaddch(map, thing->ypos, thing->xpos, thing->glyph);
    wrefresh(map);
    return thing;
}

THING* placeAt(THING* thing, int y, int x) {
    // take a thing, place it in a given location
    thing->ypos = y;
    thing->xpos = x;
    thing->under = mvwinch(map, y, x);
    mvwaddch(map, thing->ypos, thing->xpos, thing->glyph);
    wrefresh(map);
    return thing;
}

THING* newThing(ThingType type, chtype glyph) {
    // take a thing type and a glyph, create a thing and return it
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
    thing->type = type;
    thing->room = map;
    thing->glyph = glyph;
    thing->ypos = -1;
    thing->xpos = -1;
    thing->isEdible = false;
    thing->isPotable = false;
    thing->isEquippable = false;
    thing->isAggressive = false;
    thing->isDead = false;
    thing->isConfused = false;
    thing->isBlind = false;
    thing->wearingTeleportRing = false;
    thing->blindnessDuration = 0;
    thing->isHallucinating = false;
    thing->hallucinationDuration = 0;
    thing->wearingTeleportRing = false;
    thing->teleportationCycle = 0;
    thing->isAsleep = false;
    thing->isSlowed = false;
    thing->isHasted = false;
    thing->isLevitating = false;
    thing->isInCombat = false;
    thing->isCursed = false;
    thing->isInjured = false;
    thing->healingCycle = 0;
    thing->regenerates = false;
    thing->intensity = 0;
    thing->descr[0] = 0;;
    thing->ident = NULL;
    thing->typeId = -1;
    switch (glyph) {
        case '<':
            snprintf(thing->descr, sizeof thing->descr, "%s", "stair");
            break;
        case '>':
            snprintf(thing->descr, sizeof thing->descr, "%s", "stair");
            break;
        case '*':
            snprintf(thing->descr, sizeof thing->descr, "%s", "leg of lamb");
            thing->isEdible = true;
            break;
    }
    thing->gold = 0;
    thing->inInventory = false;
    thing->damage = "1d4";
    thing->hplus = 0;
    thing->dplus = 0;
    thing->armour = acValue[0];
    thing->next = things;
    things = thing;
    thing->under = 0;
    return thing;
}

