#include "main.h"

const char* scrollNames[] = {
    "scroll of confuse monster",
    "scroll of magic mapping",
    "scroll of light",
    "scroll of hold monster",
    "scroll of sleep",
    "scroll of enchant armour",
    "scroll of identify",
    "scroll of protect armour",
    "scroll of scare monster",
    "scroll of food detection",
    "scroll of teleportation",
    "scroll of enchant weapon",
    "scroll of create monster",
    "scroll of remove curse",
    "scroll of aggravate monster",
    "scroll of blank paper"
};

int sprobs[NSCROLLS] = {
    8, 5, 10, 2, 5, 8, 21, 4, 4, 7, 10, 5, 8, 1, 1, 1
};
int spt[NSCROLLS];

THING* addScroll() {
    // take a window, create a scroll and return it
    int i = pickOne(spt, NSCROLLS);
    THING* t = newThing(T_Item, '~');
    snprintf(t->descr, sizeof t->descr, "%s", "a magical scroll");
    t->typeId = i;
    return t;
}

void initScrolls() {
    // set up scrolls probability table
    spt[0] = sprobs[0];
    for (int i = 1; i < NSCROLLS; i++) {
        spt[i] = sprobs[i] + spt[i-1];
    }
    //TODO check total = 100
}

void readEffect(int i) {
    // take an inventory number, check typeId for that scroll and handle effect
    THING* t = inventory[i];
    msg("As you read the scroll, it vanishes.");
    int dx, dy;
    switch (t->typeId) {
        case S_CONFUSE_M:
            // When you strike enemies using a melee weapon there is a chance that they will become confused.
            // "Your hands begin to glow red". Next melee attack confuses a monster.
            msg("Your hands begin to glow red");
            player->canConfuse = true;
            //TODO expire? -- once only
            break;
        case S_MAGIC_MAP:
            // Its affect reads: "this scroll seems to have a map on it". It reveals all the rooms, corridors, traps, and stairs down on that dungeon level. It does not show enemies or items.
            // Reveals the entire map; objects are still hidden
            msg("Oh, now this scroll has a map on it.");
            break;
        case S_LIGHT:
            // lights a room
            // TODO
            break;
        case S_HOLD_MONS:
            // Its affect reads: "the monster freezes". It keeps monsters from moving, obviously.
            // Freezes adjacent monsters.
            // TODO
            break;
        case S_SLEEP:
            // Its affect reads: "you fall asleep". Obviously, you fall asleep for some amount of time (similar to the sleeping gas trap).
            // Sleeps reader for 4-8 turns.
            msg("you fall asleep");
            player->isAsleep = true;
            player->sleepDuration = dice2("4d2");
            break;
        case S_ENCH_ARM:
            // Its affect reads: "your armor glows blue for a moment". It increases the bonus on your armor by 1. If your armor was cursed (e.g. -3), it will no longer be so and the penalty for wearing it will be one less (e.g. -2).
            // "Your armor glows silver for a moment". Improved protection by 1.
            if (worn != NULL) {
                msg("Your armour glows silver for a moment");
                //TODO improve ac
                t->isCursed = false;
            }
            break;
        case S_IDENTIFY:
            // It identifies another item of your choosing when used.
            msg("This scroll is an identify scroll");
            //TODO identify
            break;
        case S_PROT_ARM:
            // Its affect reads: "your armor is covered by a shimmering gold shield".
            // "Your armor is covered by a shimmering gold shield". Prevents rust damage from aquators.
            msg("Your armor is covered by a shimmering gold shield");
            break;
        case S_SCARE_M:
            //
            // "When read, you hear maniacal laughter in the distance". To properly use the scroll, drop it; monsters will not walk on it.
            msg("You hear maniacal laughter in the distance.");
            break;
        case S_FOOD_DET:
            //
            // "Your nose tingles". Detects food on the current level.
            msg("Your nose tingles");
            //TODO reveal food
            break;
        case S_TELEPORT:
            // It teleports you to a random location in the current dungeon level.
            // Teleports to a random location on the map.
            mvwaddch(player->room, player->ypos, player->xpos, player->under);
            place(player);
            break;
        case S_ENCH_WPN:
            // It increases the bonus on your weapon by +0,+1. If your weapon was cursed, it will no longer be so and the penalty for weilding it will be one less.
            // effect on wielded weapon only
            // "Your (weapon) glows blue for a moment". Increases hit or damage bonus by 1.
            if (wielded == NULL) {
                msg("You feel a strange sense of loss.");
            } else {
                mvprintw(1, 0, "your %s glows blue for a moment", t->descr);
                player->hplus = wielded->hplus++;
                t->isCursed = false;
            }
            break;
        case S_CREATE_M:
            // Just like its name says, it creates a random monster (suitable for that dungeon level) adjacent to you. This monster will be hostile.
            // Summons a monster on an adjacent tile. If it fails, "you hear a faint cry of anguish in the distance".
            dy = 0;
            dx = 0;
            while (dy == 0 && dx == 0) {
                dy = dice(1, 3) - 1;
                dx = dice(1, 3) - 1;
            }
            placeAt(addMonster(0), player->ypos + dy, player->xpos + dx);
            break;
        case S_REM_CURS:
            //
            // "You feel as if somebody is watching over you". Removes curses from equipped items.
            msg("You feel as if somebody is watching over you.");
            if (worn != NULL)
                worn->isCursed = false;
            if (wielded != NULL)
                wielded->isCursed = false;
            if (left != NULL)
                left->isCursed = false;
            if (right != NULL)
                right->isCursed = false;
            break;
        case S_AGGR_MON:
            // Monsters that may have been docile are now hostile towards you.
            // "You hear a high-pitched humming noise".. Alerts all monsters on the map.
            msg("You hear a high pitched humming noise.");
            //TODO aggravate
            break;
        case S_BLNK_PAP:
            msg("This scroll seems to be blank.");
            break;
    }
}

