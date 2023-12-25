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

/* flags: Flying Mean Regenerating Invisible
Name	    Treasure	Flags	Exp	HP  AC	Damage	    Range	Notes	        Original Name
Aquator	        0	    M	    20	5    2	0d0/0d0	    9-16	Rusts armor	        Rust Monster
Bat	            0	    F	    1	1    3	1d2 	    1-7	    Flies randomly	
Centaur	        15		        17	4    4	1d2/1d5/1d5	8-15		
Dragon	        100	    M	   5000	10	-1	1d8/1d8/3d10 23+	Ranged 6d6 flame attack***	
Emu	            0       M	    2	1    7	1d2	        1-6		                    Jackal
Venus Flytrap	0	    M	    80	8 	3	special*	15-22	Traps player*	    Violet Fungi
Griffin 	    20	    MFR	   2000	13	2	4d3/3d5	    18-26
Hobgoblin	    0	    M	    3	1 	5	1d8	        1-8		
Ice monster	    0		        5	1 	9	0d0	        2-9	    Freezes player**	Floating Eye
Jabberwock	    70		       3000	15	6	2d12/2d4	22+		Purple Worm
Kestrel	        0	    MF	    1	1 	7	1d4	        1-5		Kobold
Leprechaun	    0		        10	3 	8	1d1	        7-14	Steals gold***	
Medusa	        40	    M	    200	8 	2	3d4/3d4/2d5	19+	    Confuses Hero***	Umber Hulk
Nymph	        100		        37	3 	9	0d0	        11-18	Steals unequipped magic item	
Orc	            15	    	    5	1 	6	1d8	        4-11
Phantom	        0	    I	    120	8 	3	4d4	        16-23	Invisible	        Invisible Stalker
Quagga	        0	    M	    15	3 	3	1d5/1d5	    10-17		                Quasit
Rattlesnake	    0	    M	    9	2 	3	1d6	        5-12	Reduces Strength**	Giant Ant
Snake	        0	    M	    2	1 	5	1d3	        1-10
Troll	        50	    RM	    120	6 	4	1d8/1d8/2d6	13-20		
Black Unicorn	0	    M	    190	7 	-2	1d9/1d9/2d9	17-24		                Xorn, roughly.
Vampire	        20	    RM	    350	8 	1	1d10	    21+	    Drains 1-3 Max HP (30% chance)	
Wraith	        0		        55	5 	4	1d6	        14-21	Drains level**** (15% chance)	
Xeroc	        30		        100	7 	7	4d4	        20+	    Imitates an object	    Mimic
Yeti	        30		        50	4 	6	1d6/1d6	    12-19		
Zombie	        0	    M	    6	2 	8	1d8	        4-13		
*The Venus flytrap, regardless of hit-or-miss messages, does no damage at first, and then deals one point more each time than last turn.

**Poison and Paralysis/Freezing attacks have a 1d20 saving throw. The value needed to save is 14 - ( [current player level] / 2 ). [current player level] / 2 rounds down. This throw also applies to dart traps. Saving throws for VS_BREATH and VS_DEATH are found in the source, but are not used. Rogue 3.6.3 also has an unused saving throw for VS_PETRIFICATION.

***Magical saving throw applies, using 1d20. The value needed to save is 17 - [protection level] - ( [current player level] / 2 ). Interestingly, a magical saving throw based on the player's stats is used to determine not only if a leprechaun's stealing attack succeeds, but also determines if a leprechaun drops 5x gold when killed. The player gets a magical saving throw when struck by a bouncing wand bolt, or by dragon breath. Monsters get a magical saving throw when struck by a wand bolt or magic missile. (but monsters can't have protection)

**** Level draining removes 1d10 of the player's max HP. Being level drained with 0 exp results in death. If the player is at experience level 1 with >0 exp, exp is reduced to 0.

*/

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

struct monster monsters[26] = {
    { "giant ant",  "yes" },  
    { "bat",        "no" },	      
    { "centaur",    "no" },	  
    { "dragon",     "no" },	  
    { "floating eye","sometimes" },
    { "violet fungi","yes" },
    { "goblin",     "sometimes" },	  
    { "hobgoblin",  "yes" },  
    { "ice monster","no" },
    { "jackal",     "yes" },	  
    { "kobold",     "yes" },	  
    { "leprechaun", "no" }, 
    { "mimic",      "no" },	  
    { "nymph",      "no" },	  
    { "orc",        "yes" },	      
    { "purple worm","no" },
    { "quasit",     "yes" },	  
    { "rust monster","yes" },
    { "snake",      "yes" },	  
    { "troll",      "yes" },	  
    { "umber hulk", "yes" }, 
    { "vampire",    "yes" },	  
    { "wraith",     "no" },	  
    { "xorn",       "yes" },	      
    { "yeti",       "no" },	      
    { "zombie",     "yes" },	  
};

char lvltable[] = "KJBSGHEAOZLCRQNYTWFIXUMVDP";
char wmtable[] =  "KJBSGH AOZ CRQ Y W IXU V  ";

chtype randomMonster(bool wanderer) {
    int d;
    char *table = wanderer ? wmtable : lvltable;
    do {
        d = dlevel + (rnd(10) - 5);
        if (d < 1)
            d = rnd(5) + 1;
        if (d > 26)
            d = rnd(5) + 22;
    } while (table[--d] == ' ');
    return table[d];
}


THING* addMonster(chtype kind) {
    // take a species code, create a monster and return it
    chtype k;
    THING* t = newThing(T_Sprite, 'X');
    const char *aggr;
    if (kind == 0) {
        k = randomMonster(false);
    } else {
        k = kind;
    }
    int i = k - 'A';
    switch (k) {
        case 'K':
            snprintf(t->descr, sizeof t->descr, "%s", monsters[i].name);
            t->glyph = k;
            aggr = monsters[i].aggr;
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d4";
            t->armour = 7;
            t->expAward = 1;
            break;
        case 'J':
            snprintf(t->descr, sizeof t->descr, "%s", "jackal");
            t->glyph = 'J';
            aggr = "yes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d2";
            t->armour = 7;
            t->expAward = 2;
            break;
        case 'B':
            snprintf(t->descr, sizeof t->descr, "%s", "bat");
            t->glyph = 'B';
            aggr = "no";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d2";
            t->armour = 3;
            t->expAward = 1;
            break;
        case 'S':
            snprintf(t->descr, sizeof t->descr, "%s", "snake");
            t->glyph = 'S';
            aggr = "yes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d3";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 'G':
            snprintf(t->descr, sizeof t->descr, "%s", "goblin");
            t->glyph = 'G';
            aggr = "sometimes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d6";
            t->armour = 5;
            t->expAward = 2;
            break;
        case 'H':
            snprintf(t->descr, sizeof t->descr, "%s", "hobgoblin");
            t->glyph = 'H';
            aggr = "yes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d8";
            t->armour = 5;
            t->expAward = 3;
            break;
        case 'E':
            snprintf(t->descr, sizeof t->descr, "%s", "floating eye");
            t->glyph = 'E';
            aggr = "sometimes";
            t->stats->fullHp = dice(1, 8);
            t->damage = "0d0";
            //TODO
            t->armour = 9;
            t->expAward = 5;
            t->isImmobile = true;
            break;
        case 'A':
            snprintf(t->descr, sizeof t->descr, "%s", "giant ant");
            t->glyph = 'A';
            aggr = "yes";
            t->stats->fullHp = dice(2, 8);
            t->damage = "1d6";
            t->armour = 3;
            t->expAward = 10;
            break;
        case 'I':
            snprintf(t->descr, sizeof t->descr, "%s", "ice monster");
            t->glyph = 'I';
            aggr = "no";
            t->stats->fullHp = dice(1, 8);
            t->damage = "0d0";
            t->armour = 9;
            t->expAward = 5;
            break;
        case 'O':
            snprintf(t->descr, sizeof t->descr, "%s", "orc");
            t->glyph = 'O';
            aggr = "no";
            t->stats->fullHp = dice(1, 8);
            t->damage = "1d8";
            t->armour = 6;
            t->expAward = 5;
            break;
        case 'Z':
            snprintf(t->descr, sizeof t->descr, "%s", "zombie");
            t->glyph = 'Z';
            aggr = "yes";
            t->stats->fullHp = dice(2, 8);
            t->damage = "1d8";
            t->armour = 8;
            t->expAward = 7;
            break;
        case 'L':
            snprintf(t->descr, sizeof t->descr, "%s", "leprechaun");
            t->glyph = 'L';
            aggr = "no";
            t->stats->fullHp = dice(3, 8);
            t->damage = "1d1";
            t->armour = 8;
            t->expAward = 10;
            break;
        case 'C':
            snprintf(t->descr, sizeof t->descr, "%s", "centaur");
            t->glyph = 'C';
            aggr = "no";
            t->stats->fullHp = dice(4, 8);
            t->damage = "1d6/1d6";
            t->armour = 4;
            t->expAward = 15;
            break;
        case 'R':
            snprintf(t->descr, sizeof t->descr, "%s", "rust monster");
            t->glyph = 'R';
            aggr = "yes";
            t->stats->fullHp = dice(5, 8);
            t->damage = "0d0/0d0";
            t->armour = 2;
            t->expAward = 25;
            break;
        case 'Q':
            snprintf(t->descr, sizeof t->descr, "%s", "quasit");
            t->glyph = 'Q';
            aggr = "yes";
            t->stats->fullHp = dice(3, 8);
            t->damage = "1d2/1d2/1d4";
            t->armour = 2;
            t->expAward = 35;
            break;
        case 'N':
            snprintf(t->descr, sizeof t->descr, "%s", "nymph");
            t->glyph = 'N';
            aggr = "no";
            t->stats->fullHp = dice(3, 8);
            t->damage = "0d0";
            t->armour = 9;
            t->expAward = 45;
            break;
        case 'Y':
            snprintf(t->descr, sizeof t->descr, "%s", "yeti");
            t->glyph = 'Y';
            aggr = "no";
            t->stats->fullHp = dice(4, 8);
            t->damage = "1d6/1d6";
            t->armour = 6;
            t->expAward = 50;
            break;
        case 'T':
            snprintf(t->descr, sizeof t->descr, "%s", "troll");
            t->glyph = 'T';
            aggr = "yes";
            t->stats->fullHp = dice(6, 8);
            t->damage = "1d8/1d8/2d6";
            t->armour = 4;
            t->regenerates = true;
            t->expAward = 55;
            break;
        case 'W':
            snprintf(t->descr, sizeof t->descr, "%s", "wraith");
            t->glyph = 'W';
            aggr = "yes";
            t->stats->fullHp = dice(5, 8);
            t->damage = "1d6";
            t->armour = 4;
            t->expAward = 55;
            break;
        case 'F':
            snprintf(t->descr, sizeof t->descr, "%s", "violet fungi");
            t->glyph = 'F';
            aggr = "yes";
            t->stats->fullHp = dice(8, 8);
            t->damage = "1d6";
            t->armour = 3;
            t->expAward = 85;
            break;
        case 'X':
            snprintf(t->descr, sizeof t->descr, "%s", "xorn");
            t->glyph = 'X';
            aggr = "yes";
            t->stats->fullHp = dice(7, 8);
            t->damage = "1d3/1d3/1d3/4d6";
            t->armour = 2;
            t->expAward = 120;
            break;
        case 'U':
            snprintf(t->descr, sizeof t->descr, "%s", "umber hulk");
            t->glyph = 'U';
            aggr = "yes";
            t->stats->fullHp = dice(8, 8);
            t->damage = "3d4/3d4/2d5";
            t->armour = 2;
            t->expAward = 130;
            break;
        case 'M':
            snprintf(t->descr, sizeof t->descr, "%s", "mimic");
            t->glyph = 'M';
            aggr = "no";
            t->stats->fullHp = dice(7, 8);
            t->damage = "3d4";
            t->armour = 7;
            t->expAward = 140;
            break;
        case 'V':
            snprintf(t->descr, sizeof t->descr, "%s", "vampire");
            t->glyph = 'V';
            aggr = "yes";
            t->stats->fullHp = dice(8, 8);
            t->damage = "1d10";
            t->armour = 1;
            t->expAward = 380;
            break;
        case 'D':
            snprintf(t->descr, sizeof t->descr, "%s", "dragon");
            t->glyph = 'D';
            aggr = "no";
            t->stats->fullHp = dice(10, 8);
            t->damage = "1d8/1d8/3d10";
            t->armour = -1;
            t->expAward = 9000;
            break;
        case 'P':
            snprintf(t->descr, sizeof t->descr, "%s", "purple worm");
            t->glyph = 'P';
            aggr = "no";
            t->stats->fullHp = dice(15, 8);
            t->damage = "2d12/2d4";
            t->armour = 6;
            t->expAward = 7000;
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

