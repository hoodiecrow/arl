#include "main.h"

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

int wprobs[NWEAPONS] = {
    21, 21, 16, 20, 22
};
int wpt[NWEAPONS];

THING* addWeapon(int typeId) {
    // take a typeId, create a (random, if -1) weapon and return it
    THING* t = newThing(T_Item, ')');
    if (typeId < 0) {
        typeId = pickOne(wpt, NWEAPONS);
        int k;
        if ((k = rnd(100)) < 10) {
            t->isCursed = true;
            t->hplus -= rnd(3)+1;
        } else if (k < 15) {
            t->hplus += rnd(3)+1;
        }
    }
    snprintf(t->descr, sizeof t->descr, "%s", weaponNames[typeId]);
    t->damage = weaponDamage[typeId];
    t->typeId = typeId;
    return t;
}

void initWeapons() {
    // set up weapons probability table
    wpt[0] = wprobs[0];
    for (int i = 1; i < NWEAPONS; i++) {
        wpt[i] = wprobs[i] + wpt[i-1];
    }
    //TODO check total = 100
}

void wieldEffect(int i) {
    // take an inventory number, set wielded to that weapon
    //TODO
    wielded = inventory[i];
    player->damage = wielded->damage;
    player->hplus = wielded->hplus;
    player->dplus = wielded->dplus;
}

void unwield() {
    wielded = NULL;
    player->damage = "1d4";
    player->hplus = 0;
    player->dplus = 0;
}
