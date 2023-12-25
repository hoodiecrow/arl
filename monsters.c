#include "main.h"

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

struct monster monsters[26] = {
    { "giant ant",  "yes",      10,     2,  3,  "1d6" },  
    { "bat",        "no",       1,      1,  3,  "1d2" },	      
    { "centaur",    "no",       15,     4,  4,  "1d6/1d6" },	  
    { "dragon",     "no" ,      9000,   10, -1, "1d8/1d8/3d10" },	  
    { "floating eye","sometimes",5,     1,  9,  "0d0" },
    { "violet fungi","yes",     85,     8,  3,  "000d0" },
    { "goblin",     "sometimes",2,      1,  5,  "1d6" },	  
    { "hobgoblin",  "yes",      3,      1,  5,  "1d8" },  
    { "ice monster","no",       5,      1,  9,  "0d0" },
    { "jackal",     "yes",      2,      1,  7,  "1d2" },	  
    { "kobold",     "yes" ,     1,      1,  7,  "1d4" },	  
    { "leprechaun", "no",       10,     3,  8,  "1d1" }, 
    { "mimic",      "no",       140,    7,  7,  "3d4" },	  
    { "nymph",      "no",       40,     3,  9,  "0d0" },	  
    { "orc",        "yes",      5,      1,  6,  "1d8" },
    { "purple worm","no",       7000,   15, 6,  "2d12/2d4" },
    { "quasit",     "yes",      35,     3,  2,  "1d2/1d2/1d4" },	  
    { "rust monster","yes",     25,     5,  2,  "0d0/0d0" },
    { "snake",      "yes",      3,      1,  5,  "1d3" },	  
    { "troll",      "yes",      55,     6,  4,  "1d8/1d8/2d6" },	  
    { "umber hulk", "yes",      130,    8,  2,  "3d4/3d4/2d5" }, 
    { "vampire",    "yes",      380,    8,  1,  "1d10" },	  
    { "wraith",     "no",       55,     5,  4,  "1d6" },	  
    { "xorn",       "yes",      120,    7,  -2, "1d3/1d3/1d3/4d6" },	      
    { "yeti",       "no",       50,     4,  6,  "1d6/1d6" },	      
    { "zombie",     "yes",      7,      2,  8,  "1d8" },	  
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
    snprintf(t->descr, sizeof t->descr, "%s", monsters[i].name);
    t->glyph = k;
    aggr = monsters[i].aggr;
    t->stats->fullHp = dice(monsters[i].level, 8);
    t->damage = monsters[i].damage;
    t->armour = monsters[i].armour;
    t->expAward = monsters[i].expawd;
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

