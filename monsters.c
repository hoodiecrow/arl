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

	/* Name	     CARRY	FLAG        str,    exp, lvl, amr, hpt, dmg *//*
"giant ant",     0, 	ISMEAN,	    { _x,    10,   2,   3, ___, "1d6" } },
"bat",	         0, 	0,	        { _x,     1,   1,   3, ___, "1d2" } },
"centaur",	     15,	0,	        { _x,    15,   4,   4, ___, "1d6/1d6" } },
"dragon",	    100,	ISGREED,    { _x,   9000, 10,  -1, ___, "1d8/1d8/3d10" } },
"floating eye",  0, 	0,	        { _x,     5,   1,   9, ___, "0d0" } },
"violet fungi",  0, 	ISMEAN,	    { _x,    85,   8,   3, ___, "000d0" } },
"gnome",	     10,	0,	        { _x,     8,   1,   5, ___, "1d6" } },
"hobgoblin",	 0, 	ISMEAN,	    { _x,     3,   1,   5, ___, "1d8" } },
"invisible stalker",0,  ISINVIS,    { _x,   120,   8,   3, ___, "4d4" } },
"jackal",	     0, 	ISMEAN,	    { _x,     2,   1,   7, ___, "1d2" } },
"kobold",	     0, 	ISMEAN,	    { _x,     1,   1,   7, ___, "1d4" } },
"leprechaun",	 0, 	0,	        { _x,    10,   3,   8, ___, "1d1" } },
"mimic",	     30,	0,	        { _x,   140,   7,   7, ___, "3d4" } },
"nymph",	     100,	0,	        { _x,    40,   3,   9, ___, "0d0" } },
"orc",	         15,	ISBLOCK,    { _x,     5,   1,   6, ___, "1d8" } },
"purple worm",   70,	0,	        { _x,   7000, 15,   6, ___, "2d12/2d4" } },
"quasit",	     30,	ISMEAN, 	{ _x,    35,   3,   2, ___, "1d2/1d2/1d4" } },
"rust monster",  0, 	ISMEAN, 	{ _x,    25,   5,   2, ___, "0d0/0d0" } },
"snake",	     0, 	ISMEAN,	    { _x,     3,   1,   5, ___, "1d3" } },
"troll",	     50,	ISREGEN|ISMEAN,{ _x, 55,   6,   4, ___, "1d8/1d8/2d6" } },
"umber hulk",	 40,	ISMEAN, 	{ _x,   130,   8,   2, ___, "3d4/3d4/2d5" } },
"vampire",	     20,	ISREGEN|ISMEAN,{ _x,380,   8,   1, ___, "1d10" } },
"wraith",	     0, 	0,	        { _x,    55,   5,   4, ___, "1d6" } },
"xorn",	         0, 	ISMEAN, 	{ _x,   120,   7,  -2, ___, "1d3/1d3/1d3/4d6" } },
"yeti",	         30,	0,	        { _x,    50,   4,   6, ___, "1d6/1d6" } },
"zombie",	     0, 	ISMEAN,	    { _x,     7,   2,   8, ___, "1d8" } }
*/
THING* addMonster(chtype kind) {
    // take a window, create a monster and return it
    int i;
    THING* t = newThing(T_Sprite, 'X');
    const char *aggr;
    if (kind == 0) {
        if (dlevel == 1) 
            i = rnd(6);
        else if (dlevel == 2) 
            i = rnd(7);
        else
            i = 0;
    } else {
        i = kind - 'A';
    }
    switch (i) {
        case 0:
            t->descr = "bat";
            t->glyph = 'B';
            aggr = "no";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d2";
            t->armour = 3;
            t->expAward = 1;
            break;
        case 1:
            t->descr = "floating eye";
            t->glyph = 'E';
            aggr = "sometimes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "0d0";
            //TODO
            t->armour = 9;
            t->expAward = 5;
            t->isImmobile = true;
            break;
        case 2:
            t->descr = "goblin";
            t->glyph = 'G';
            aggr = "sometimes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d6";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 3:
            t->descr = "hobgoblin";
            t->glyph = 'H';
            aggr = "yes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d8";
            t->armour = 5;
            t->expAward = 3;
            break;
        case 5:
            t->descr = "snake";
            t->glyph = 'S';
            aggr = "yes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d3";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 6:
            t->descr = "ice monster";
            t->glyph = 'I';
            aggr = "no";
            t->stats->fullHp = dice(8, 8);
            t->damage = "4d4";
            t->armour = 3;
            t->expAward = 120;
            break;
    }
    t->hplus = 0;
    t->dplus = 0;
    t->stats->currHp = t->stats->fullHp;
    if (strcmp(aggr, "sometimes") == 0) {
        t->isAggressive = rnd(2)==1?true:false;
    } else if (strcmp(aggr, "no") == 0) {
        t->isAggressive = false;
    } else if (strcmp(aggr, "yes") == 0) {
        t->isAggressive = true;
    }
    return t;
}

