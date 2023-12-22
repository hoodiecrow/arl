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
    "scroll of gold detection",
    "scroll of teleportation",
    "scroll of enchant weapon",
    "scroll of create monster",
    "scroll of remove curse",
    "scroll of aggravate monster",
    "scroll of blank paper"
};

THING* addScroll(WINDOW* win) {
    // take a window, create a scroll and return it
    int i = random() % NSCROLLS;
    int y, x;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Item, '~', y, x);
    t->descr = "a magical scroll";
    t->typeId = i;
    return t;
}

void readEffect(int i) {
    // take an inventory number, check typeId for that scroll and handle effect
    THING* t = inventory[i];
    msg("As you read the scroll, it vanishes.");
    switch (t->typeId) {
        case S_CONFUSE_M:
            // When you strike enemies using a melee weapon there is a chance that they will become confused.
            msg("Your hands begin to glow red");
            player->canConfuse = true;
            //TODO expire?
            break;
        case S_MAGIC_MAP:
            // Its affect reads: "this scroll seems to have a map on it". It reveals all the rooms, corridors, traps, and stairs down on that dungeon level. It does not show enemies or items.
            msg("Oh, now this scroll has a map on it.");
            break;
        case S_LIGHT:
            break;
        case S_HOLD_MONS:
            // Its affect reads: "the monster freezes". It keeps monsters from moving, obviously.
            break;
        case S_SLEEP:
            // Its affect reads: "you fall asleep". Obviously, you fall asleep for some amount of time (similar to the sleeping gas trap).
            player->isAsleep = true;
            mvaddstr(1, 0, "you fall asleep"); clrtoeol();
            player->sleepDuration = 50;
            break;
        case S_ENCH_ARM:
            // Its affect reads: "your armor glows blue for a moment". It increases the bonus on your armor by 1. If your armor was cursed (e.g. -3), it will no longer be so and the penalty for wearing it will be one less (e.g. -2).
            if (worn != NULL) {
                msg("Your armour glows faintly for a moment");
                //TODO reduce ac
                //TODO remove curse
            }
            break;
        case S_IDENTIFY:
            // It identifies another item of your choosing when used.
            msg("This scroll is an identify scroll");
            //TODO identify
            break;
        case S_PROT_ARM:
            // Its affect reads: "your armor is covered by a shimmering gold shield".
            break;
        case S_SCARE_M:
            msg("You hear maniacal laughter in the distance.");
            break;
        case S_GOLD_DET:
            break;
        case S_TELEPORT:
            // It teleports you to a random location in the current dungeon level.
            mvwaddch(player->room, player->ypos, player->xpos, player->under);
            int y, x;
            getOpenLocation(player->room, &y, &x);
            player->ypos = y;
            player->xpos = x;
            player->under = ' ';
            present(player);
            break;
        case S_ENCH_WPN:
            // It increases the bonus on your weapon by +0,+1. If your weapon was cursed, it will no longer be so and the penalty for weilding it will be one less.
            // effect on wielded weapon only
            if (wielded == NULL) {
                msg("You feel a strange sense of loss.");
            } else {
                mvaddstr(1, 0, "your weapon glows blue for a moment");
                wielded->wplus += random() % 2;
                // TODO cursed status
            }
            break;
        case S_CREATE_M:
            // Just like its name says, it creates a random monster (suitable for that dungeon level) adjacent to you. This monster will be hostile.
            // TODO make it adjacent
            addMonster(t->room);
            break;
        case S_REM_CURS:
            if (worn != NULL)
                worn->isCursed = false;
            if (wielded != NULL)
                wielded->isCursed = false;
            if (left != NULL)
                left->isCursed = false;
            if (right != NULL)
                right->isCursed = false;
            msg("You feel as if somebody is watching over you.");
            break;
        case S_AGGR_MON:
            // Monsters that may have been docile are now hostile towards you.
            msg("You hear a high pitched humming noise.");
            //TODO aggravate
            break;
        case S_BLNK_PAP:
            msg("This scroll seems to be blank.");
            break;
    }
}

