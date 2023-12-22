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
    int y, x;
    getOpenLocation(&y, &x);
    THING* t = newThing(T_Item, ')', y, x);
    t->descr = weaponNames[i];
    t->damage = weaponDamage[i];
    t->typeId = i;
    return t;
}

void wieldEffect(int i) {
    // take an inventory number, set wielded to that weapon
    //TODO
    THING* t = inventory[i];
    wielded = t;
    player->damage = wielded->damage;
}

