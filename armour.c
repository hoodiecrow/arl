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

THING* addArmour(int typeId) {
    // take a typeId, create a (random, if -1) armour and return it
    THING* t = newThing(T_Item, ']');
    if (typeId < 0) {
        typeId = pickOne(apt, NARMOURS);
        int k;
        if ((k = rnd(100)) < 20) {
            t->isCursed = true;
            t->armour += rnd(3)+1;
        } else if (k < 28) {
            t->armour -= rnd(3)+1;
        }
    }
    snprintf(t->descr, sizeof t->descr, "%s", armourNames[typeId]);
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
    // take an inventory number, set worn to that armour
    worn = inventory[i];
    // copy worn armour value to player
    player->armour = worn->armour;
    // TODO lesser or greater quality variants
}

