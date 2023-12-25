#include "main.h"

THING* addTrap() {
    int i = rnd(6);
    THING* t = newThing(T_Item, ' ');
    snprintf(t->descr, sizeof t->descr, "%s", "a trap");
    t->isFound = false;
    t->typeId = i;
    return t;
}

int ntraps;
THING* traps[MAXTRAPS];

/* in part adapted from Rogue3.6.3 */
void initTraps() {
    if (rnd(10) < dlevel) {
        ntraps = rnd(dlevel/4)+1;
        if (ntraps > MAXTRAPS)
            ntraps = MAXTRAPS;
        for (int i = 0; i < ntraps; i++) {
            traps[i] = addTrap();
            place(traps[i]);
        }
	}
}
