#include "main.h"

/* adapted from Rogue3.6.3 */
THING *trapAt(int y, int x) {
    THING** tp;
    THING** ep;
    ep = &traps[ntraps];
    for (tp = traps; tp < ep; tp++)
        if ((*tp)->ypos == y && (*tp)->xpos == x)
            break;
    return *tp;
}

/* adapted from Rogue3.6.3 */
char *trapName(int typeId) {
    switch (typeId) {
        case TRAPDOOR:
            return "You found a trapdoor.";
        case BEARTRAP:
            return "You found a beartrap.";
        case SLEEPTRAP:
            return "You found a sleeping gas trap.";
        case ARROWTRAP:
            return "You found an arrow trap.";
        case TELTRAP:
            return "You found a teleport trap.";
        case DARTTRAP:
            return "You found a poison dart trap.";
        default:
            return NULL;
    }
}

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
