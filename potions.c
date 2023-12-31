#include "main.h"

/* adapted from Rogue3.6.3 */
const char *potionNames[] = {
    "potion of confusion",
    "potion of hallucination",
    "potion of poison",
    "potion of increase strength",
    "potion of see invisible",
    "potion of healing",
    "potion of detect monster",
    "potion of detect things",
    "potion of raise level",
    "potion of extra healing",
    "potion of haste self",
    "potion of restore strength",
    "potion of blindness",
    "potion of levitation",
};

const char* p_colours[NPOTIONS];

int pprobs[NPOTIONS] = {
    8, 10, 8, 15, 2, 15, 6, 6, 2, 5, 4, 14, 4, 1
};
int ppt[NPOTIONS];

/* from Rogue3.6.3 */
char *rainbow[] = {
    "red",
    "blue",
    "green",
    "yellow",
    "black",
    "brown",
    "orange",
    "pink",
    "purple",
    "grey",
    "white",
    "silver",
    "gold",
    "violet",
    "clear",
    "vermilion",
    "ecru",
    "turquoise",
    "magenta",
    "amber",
    "topaz",
    "plaid",
    "tan",
    "tangerine"
};

#define NCOLOURS (24)

THING* addPotion() {
    // create a potion and return it
    int i = pickOne(ppt, NPOTIONS);
    THING* t = newThing(T_Item, '!');
    snprintf(t->descr, sizeof t->descr, "%s %s", p_colours[i], "potion");
    t->typeId = i;
    return t;
}

void initPotions() {
    // set up potions probability table
    ppt[0] = pprobs[0];
    for (int i = 1; i < NPOTIONS; i++) {
        ppt[i] = pprobs[i] + ppt[i-1];
    }
    //TODO check total = 100
    // match potion types with colours
    int j;
    bool used[NCOLOURS];
    for (int i = 0; i < NCOLOURS; i++)
        used[i] = false;
    for (int i = 0; i < NPOTIONS; i++) {
        do
            j = rnd(NCOLOURS);
        while (used[j]);
        used[j] = true;
        p_colours[i] = rainbow[j];
        //p_know[i] = false;
        //p_guess[i] = NULL;
    }
}

// TODO needs an overhaul for the wiki info
void drinkEffect(int i) {
    // take an inventory number, check typeId for that potion and handle effect
    THING* t = inventory[i];
    //TODO replace description
    //TODO timer for (some) effects
    switch (t->typeId) {
        case P_BLIND:
            // Its effect reads: "a cloak of darkness falls around you". You are unable to see enemies, items, and the dungeon around you.
            // Blinds player for 807-892 turns
            // TODO
            player->isBlind = true;
            player->blindnessDuration = rnd(892-807) + 807;
            break;
        case P_CONFUS:
            // Confuses the player for 19-21 turns 
            msg("you feel confused");
            player->isConfused = true;
            player->confusionDuration = 20;
            break;
        case P_DET_MON:
            // When read [sic] no text appears. However, it reveals all monsters on the current dungeon level.
            // Reveals monsters on the map.
            // TODO
            break;
        case P_DET_THG:
            // When read no text appears. However, it reveals all items and gold present in the current dungeon level.
            // Reveals magic items on the map.
            // TODO
            break;
        case P_EXT_HEAL:
            // Heals 1d8 per character level. Increase max HP by 1, or by 2 if you are at full health. Cures blindness and hallucination.
            msg("you begin to feel much better");
            player->stats->currHp += dice(player->stats->level, 8);
            player->stats->fullHp++;
            if (player->stats->currHp > player->stats->fullHp) {
                player->stats->fullHp++;
                player->stats->currHp = player->stats->fullHp;
            }
            break;
        case P_HALLUC:
            // Its effect reads: "oh wow, everything seems so cosmic". When quaffed all enemies and items will change symbol from turn to turn. When its effect wears off it reads: "everything looks SO boring now".
            // Causes hallucinations for 850 turns - can't recognize monsters or items
            // TODO
            player->isHallucinating = true;
            player->hallucinationDuration = 850;
            break;
        case P_HASTE:
            //  It may appear as though you are moving at your normal speed but you can sometimes move twice before a monster moves once.
            //  Hastens player for 4-8 turns.
            mvaddstr(1, 0, "you feel yourself moving much faster");
            player->hasteDuration = dice2("4d2");
            break;
        case P_HEALING:
            // Its effect reads: "you begin to feel better". It restores a number of Hp (13).
            // Heals 1df per character level. Increase max HP by 1 if you are at full health. Cures blindness, but not hallucination.
            mvaddstr(1, 0, "you begin to feel better"); clrtoeol();
            player->stats->currHp += dice(player->stats->level, 4);
            if (player->stats->currHp > player->stats->fullHp) {
                player->stats->fullHp++;
                player->stats->currHp = player->stats->fullHp;
            }
            player->blindnessDuration = 0;
            break;
        case P_INC_STR:
            // When quaffed your current strength increases by one (like a potion of restore strength), however, if you strength is at max, your maximum strength increases by one.
            // Increases strength by 1
            mvaddstr(1, 0, "you feel stronger now, what bulging muscles!");
            player->stats->currStrength++;
            if (player->stats->currStrength > player->stats->fullStrength)
                player->stats->fullStrength++;
            break;
        case P_LEVIT:
            // While levitating you cannot fall into traps or pickup items.
            // Levitates for 29-32 turns
            mvaddstr(1, 0, "you start to float in the air");
            player->isLevitating = true;
            player->levitationDuration = rnd(32-29) + 29;
            break;
        case P_POISON:
            // Reduces strength by 1-3 points. Cures hallucination.
            mvaddstr(1, 0, "you feel very sick now");
            player->stats->currStrength -= dice(1, 3);
            player->hallucinationDuration = 0;
            break;
        case P_RS_LVL:
            //  You instantly attain the next level (Exp automatically adjusted).
            //  Increases experience level by 1.
            mvaddstr(1, 0, "you suddenly feel much more skillful");
            player->stats->level++;
            player->stats->exp = expForLevel(player->stats->level);
            player->stats->fullHp += hpIncrForLevel(player->stats->level);
            break;
        case P_REST_STR:
            // Your current strength increases by one up to your maximum strength.
            // "Hey, this tastes great. It makes you feel warm all over". Restores strength to maximum.
            mvaddstr(1, 0, "this tastes great, you feel warm all over");
            player->stats->currStrength++;
            if (player->stats->currStrength > player->stats->fullStrength)
                player->stats->currStrength--;
            break;
        case P_SEE_INVIS:
            // Its effect reads: "hmm, this potion tastes like slime-mold juice". 
            // "This potion tastes like slime mold juice". Reveals Phantoms. Cures blindness.
            // TODO
            player->blindnessDuration = 0;
            break;
    }
}

