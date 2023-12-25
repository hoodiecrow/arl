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

THING* addWeapon() {
    // take a window, create a weapon and return it
    int i = rnd(NWEAPONS);
    THING* t = newThing(T_Item, ')');
    snprintf(t->descr, sizeof t->descr, "%s", weaponNames[i]);
    t->damage = weaponDamage[i];
    int k;
    if ((k = rnd(100)) < 10) {
        t->isCursed = true;
        t->hplus -= rnd(3)+1;
    } else if (k < 15) {
        t->hplus += rnd(3)+1;
    }
    t->typeId = i;
    return t;
}

THING* initWeapon(THING* t, int typeId) {
    snprintf(t->descr, sizeof t->descr, "%s", weaponNames[typeId]);
    t->isCursed = false;
    t->damage = weaponDamage[typeId];
    t->typeId = typeId;
    return t;
}

void wieldEffect(int i) {
    // take an inventory number, set wielded to that weapon
    //TODO
    THING* t = inventory[i];
    wielded = t;
    player->damage = wielded->damage;
    player->hplus = wielded->hplus;
    player->dplus = wielded->dplus;
}

