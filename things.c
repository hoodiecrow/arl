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

void present(THING* sprite) {
    // take a sprite, show its glyph
    mvwaddch(sprite->room, sprite->ypos, sprite->xpos, sprite->glyph);
    wrefresh(sprite->room);
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
    thing->armour = acValue[0];
    thing->next = things;
    things = thing;
    thing->under = ' ';
    present(thing);
    return thing;
}

