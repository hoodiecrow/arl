#include "main.h"

/* adapted from Rogue3.6.3 */
const char *armourNames[] = {
    "leather armour",
    "ring mail",
    "studded leather armor",
    "scale mail",
    "chain mail",
    "splint mail",
    "banded mail",
    "plate mail",
};

int acValue[] = {
    8,
    7,
    7,
    6,
    5,
    4,
    4,
    3
};

int aprobs[NARMOURS] = {
    20, 15, 15, 13, 12, 10, 10, 5
};
int apt[NARMOURS];

THING* addArmour() {
    // create an armour and return it
    // avoid getting the "none" armour
    int i = pickOne(apt, NARMOURS);
    THING* t = newThing(T_Item, ']');
    snprintf(t->descr, sizeof t->descr, "%s", armourNames[i]);
    t->armour = acValue[i];
    int k;
    if ((k = rnd(100)) < 20) {
        t->isCursed = true;
        t->armour += rnd(3)+1;
    } else if (k < 28) {
        t->armour -= rnd(3)+1;
    }
    t->typeId = i;
    return t;
}

THING* initArmour(THING* t, int typeId) {
    snprintf(t->descr, sizeof t->descr, "%s", armourNames[typeId]);
    t->isCursed = false;
    t->armour = acValue[typeId];
    t->typeId = typeId;
    return t;
}

void initArmours() {
    // set up armours probability table
    apt[0] = aprobs[0];
    for (int i = 1; i < NARMOURS; i++) {
        apt[i] = aprobs[i] + apt[i-1];
    }
    //TODO check total = 100
}

void wearEffect(int i) {
    // take an inventory number, set worn to that armour, copy armour value to player
    THING* t = inventory[i];
    worn = t;
    player->armour = worn->armour;
    // TODO lesser or greater quality variants
}

