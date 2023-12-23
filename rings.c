#include "main.h"

const char *ringNames[] = {
    "ring of protection",		 
    "ring of add strength",	 
    "ring of sustain strength",
    "ring of searching",
    "ring of see invisible",	 
    "ring of adornment",		 
    "ring of aggravate monster",
    "ring of dexterity",		 
    "ring of increase damage", 
    "ring of regeneration",	 
    "ring of slow digestion", 
    "ring of teleportation",	 
    "ring of stealth",
};

int rprobs[NRINGS] = {
     9, 9, 5, 10, 10, 1, 11, 8, 8, 4, 9, 9, 7,
};
int rpt[NRINGS];

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
    THING* t = newThing(T_Item, ':');
    t->descr = descrs[i];
    t->isEquippable = true;
    //TODO if dropped after being identified, should be true
    t->typeId = i;
    return t;
}

void initRings() {
    // set up rings probability table
    rpt[0] = rprobs[0];
    for (int i = 1; i < NRINGS; i++) {
        rpt[i] = rprobs[i] + rpt[i-1];
    }
    //TODO check total = 100
}

void equipEffect(int i) {
    // take an inventory number, check typeId for that ring and handle effect
    // TODO Rings may be worn on either the left or right hand with no change in effect. While worn, however, they will make you hungrier quicker.
    THING* t = inventory[i];
    switch (t->typeId) {
        case R_PROTECT:
            // Adds to defense and magical saving throws
        case R_ADDSTR:
            // Adds to strength
            t->intensity = rnd(3);
            if (t->intensity == 0) {
                t->isCursed = true;
                t->intensity = -1;
            }
            break;
        case R_SUSTSTR:
            // Prevents poison from reducing strength
            break;
        case R_SEARCH:
            // Helps detect secret doors and traps.
            break;
        case R_SEEINVIS:
            // Reveals Phantoms.
            break;
        case R_NOP:
            // Worth 10 gold.
            break;
        case R_AGGR:
            // Cursed. Causes monsters to attack more aggressively.
            t->isCursed = true;
            break;
        case R_ADDHIT:
            // Improves weapon accurracy.
        case R_ADDDAM:
            // Increases weapon damage.
            t->intensity = rnd(3);
            if (t->intensity == 0) {
                t->isCursed = true;
                t->intensity = -1;
            }
            break;
            break;
        case R_REGEN:
            // Heals 1 hp per turn
            break;
        case R_DIGEST:
            // Reduces food consumption by about 50%. Two rings can cancel out food consumption.
            break;
        case R_TELEPORT:
            // When put on, you will be randomly teleported to another location in the current dungeon level every few actions. This is a cursed ring and cannot be removed until uncursed.
            // Cursed. Randomly teleports the player around the map.
            t->isCursed = true;
            player->wearingTeleportRing = true;
            break;
        case R_STEALTH:
            // Allows player to move without rousing sleeping monsters.
            break;
    }
}

