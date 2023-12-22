#include "main.h"

const char* monsterNames[26] = {
    "giant ant",    // A
    "bat",          // B
    "centaur",      // C
    "dragon",       // D
    "floating eye", // E
    "violet fungi", // F
    "goblin",       // G
    "hobgoblin",    // H
    "ice monster",  // I
    "jackal",       // J
    "kobold",       // K
    "leprechaun",   // L
    "mimic",        // M
    "nymph",        // N
    "orc",          // O
    "purple worm",  // P
    "quasit",       // Q
    "rust monster", // R
    "snake",        // S
    "troll",        // T
    "umber hulk",   // U
    "vampire",      // V
    "wraith",       // W
    "xorn",         // X
    "yeti",         // Y
    "zombie"        // Z
};

THING* addMonster(WINDOW* win) {
    // take a window, create a monster and return it
    int y, x, i;
    getOpenLocation(win, &y, &x);
    THING* t = newThing(win, T_Sprite, 'X', y, x);
    const char *aggr;
    if (dlevel == 1) 
        i = random() % 6;
    else if (dlevel == 2) 
        i = random() % 7;
    else
        i = 0;
    switch (i) {
        case 0:
            t->descr = "bat";
            t->glyph = 'B';
            aggr = "sometimes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d2";
            t->armour = 3;
            t->expAward = 2;
            break;
        case 1:
            t->descr = "floating eye";
            t->glyph = 'E';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = dice(1, 8);
            t->damage = "0d0";
            //TODO
            t->armour = 9;
            t->expAward = 2;
            t->isImmobile = true;
            break;
        case 2:
            t->descr = "goblin";
            t->glyph = 'G';
            aggr = "sometimes";
            t->attack = 2;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d6";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 3:
            t->descr = "hobgoblin";
            t->glyph = 'H';
            aggr = "yes";
            t->attack = 3;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d8";
            t->armour = 5;
            t->expAward = 3;
            break;
        case 4:
            t->descr = "kobold";
            t->glyph = 'K';
            aggr = "yes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d4";
            t->armour = 7;
            t->expAward = 2;
            break;
        case 5:
            t->descr = "snake";
            t->glyph = 'S';
            aggr = "yes";
            t->attack = 1;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d3";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 6:
            t->descr = "ice monster";
            t->glyph = 'I';
            aggr = "no";
            t->attack = 4;
            t->stats->fullHp = dice(1, 8);
            t->damage = "4d4";
            t->armour = 5;
            t->expAward = 5;
            break;
    }
    t->wplus = 0;
    t->stats->currHp = t->stats->fullHp;
    if (strcmp(aggr, "sometimes") == 0) {
        t->isAggressive = random()%2==1?true:false;
    } else if (strcmp(aggr, "no") == 0) {
        t->isAggressive = false;
    } else if (strcmp(aggr, "yes") == 0) {
        t->isAggressive = true;
    }
    return t;
}

