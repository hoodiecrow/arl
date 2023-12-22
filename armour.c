#include "main.h"

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

void wearEffect(int i) {
    // take an inventory number, set worn to that armour, copy armour value to player
    THING* t = inventory[i];
    worn = t;
    player->armour = worn->armour;
    // TODO lesser or greater quality variants
}

