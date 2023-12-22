#include "main.h"

const char *ringNames[] = {
    "ring of adornment",
    "ring of dexterity",
    "ring of maintain armor",
    "ring of see invisible",
    "ring of teleportation",
};

THING* addRing() {
    // create a ring and return it
    int i = rnd(NRINGS);
    const char *descrs[] = {
        "diamond ring",
        "emerald ring",
        "ruby ring",
        "opal ring",
        "sapphire ring",
    };
    int y, x;
    getOpenLocation(&y, &x);
    THING* t = newThing(T_Item, ':', y, x);
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->typeId = i;
    return t;
}

void equipEffect(int i) {
    // take an inventory number, check typeId for that ring and handle effect
    // TODO Rings may be worn on either the left or right hand with no change in effect. While worn, however, they will make you hungrier quicker.
    THING* t = inventory[i];
    switch (t->typeId) {
        case R_ADORN:
            // no special effect
            break;
        case R_DEXT:
            // When put on, your dexterity is adjusted by the ring's stated modifier.
            break;
        case R_MNT_ARM:
            // When put on, your armor cannot be damaged by traps or enemies (such as the aquator).
            break;
        case R_SEE_INVIS:
            // When put on, you will be able to see invisible enemies.
            break;
        case R_TELEPORT:
            // When put on, you will be randomly teleported to another location in the current dungeon level every few actions. This is a cursed ring and cannot be removed until uncursed.
            t->isCursed = true;
            player->wearingTeleportRing = true;
            break;
    }
}

