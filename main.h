#ifndef arl_main_h
#define arl_main_h

#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum THINGTYPES {
	T_POTION,
	T_SCROLL,
	T_FOOD,
	T_WEAPON,
	T_ARMOUR,
	T_RING,
	T_STICK,
	NTHINGS
};

enum STICKTYPES {
	WS_LIGHT,
	WS_HIT,
	WS_ELECT,
	WS_FIRE,
	WS_COLD,
	WS_POLYMORPH,
	WS_MISSILE,
	WS_HASTE_M,
	WS_SLOW_M,
	WS_DRAIN,
	WS_NOP,
	WS_TELAWAY,
	WS_TELTO,
	WS_CANCEL,
	NSTICKS
};

enum WEAPONTYPES {
	W_MACE,
	W_LONG,
	W_DAGG,
	W_2HSW,
	W_SPEAR,
	NWEAPONS
};

enum ARMOURTYPES {
	A_NONE,
	A_LEATH,
	A_RINGM,
	A_SCALE,
	A_CHAIN,
	A_SPLINT,
	A_BANDED,
	A_PLATE,
	NARMOURS
};

enum SCROLLTYPES {
	S_CONFUSE_M,
	S_MAGIC_MAP,
	S_LIGHT,
	S_HOLD_MONS,
	S_SLEEP,
	S_ENCH_ARM,
	S_IDENTIFY,
	S_PROT_ARM,
	S_SCARE_M,
	S_FOOD_DET,
	S_TELEPORT,
	S_ENCH_WPN,
	S_CREATE_M,
	S_REM_CURS,
	S_AGGR_MON,
	S_BLNK_PAP,
	NSCROLLS
};

enum RINGTYPES {
	R_PROTECT, 
        R_ADDSTR,
        R_SUSTSTR, 
        R_SEARCH,
        R_SEEINVIS,
        R_NOP,
        R_AGGR,
        R_ADDHIT,
        R_ADDDAM,
        R_REGEN,
        R_DIGEST,
        R_TELEPORT,
        R_STEALTH,
	NRINGS
};

enum POTIONTYPES {
	P_CONFUS,
	P_HALLUC,
	P_POISON,
	P_INC_STR,
	P_SEE_INVIS,
	P_HEALING,
	P_DET_MON,
	P_DET_THG,
	P_RS_LVL,
	P_EXT_HEAL,
	P_HASTE,
	P_REST_STR,
	P_BLIND,
	P_LEVIT,
	NPOTIONS
};

typedef enum {
    T_Sprite,
    T_Item,
    T_Structure,
} ThingType;

typedef struct STATS {
    int currStrength;
    int fullStrength;
    long exp;
    int level;
    int currHp;
    int fullHp;
} STATS;

typedef struct THING {
    ThingType type;
    chtype glyph;
    WINDOW* room;
    int ypos;
    int xpos;
    char descr[60];
    const char* ident;
    int typeId;
    int gold;
    bool inInventory;
    bool isEdible;
    bool isPotable;
    bool isEquippable;
    bool isIdentified;
    bool isAggressive;
    bool isImmobile;
    bool isInCombat;
    bool isCursed;
    bool isInjured;
    bool isDead;
    bool isCanceled;
    bool isInvisible;
    bool isConfused;
    int confusionDuration;
    bool isBlind;
    int blindnessDuration;
    bool isHallucinating;
    int hallucinationDuration;
    bool canConfuse;
    bool isSlowed;
    bool isHasted;
    int hasteDuration;
    bool isLevitating;
    int  levitationDuration;
    bool isAsleep;
    int sleepDuration;
    bool wearingTeleportRing;
    int teleportationCycle;
    int healingCycle;
    bool regenerates;
    int armour;
    struct THING* next;
    int intensity;
    int expAward;
    const char* damage;
    int hplus;
    int dplus;
    chtype under;
    STATS *stats;
    int ncharges;
} THING;

typedef struct TILE {
	bool isFloor;
	bool isBearTrap;
	THING *item;
} TILE;

#define INVENTORY_SIZE 15

extern int inventoryFill;
extern THING* inventory[];
extern bool allowedIndices[];
extern THING *things, *worn, *right, *left, *wielded, *player;
extern int deltaY, deltaX;
extern int dlevel;
extern WINDOW* map;
extern const char* monsterNames[];
extern const char *armourNames[];
extern int acValue[];

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

THING* newThing(ThingType type, chtype glyph);

THING* addMonster(chtype kind);
THING* addWeapon();
THING* initWeapon(THING* t, int typeId);
THING* addArmour();
THING* initArmour(THING* t, int typeId);
void initArmours();
THING* addStick();
void initSticks();
THING* addScroll();
void initScrolls();
THING* addRing();
void initRings();
THING* addPotion();
void initPotions();
THING* addGold();
THING* addFood();
int pickOne(int pt[], int n);
void getDir();
void freeObject(THING* o);
void wieldEffect(int i);
void zapEffect(int i);
void readEffect(int i);
void equipEffect(int i);
void wearEffect(int i);
void drinkEffect(int i);
THING* present(THING* thing);
int rnd(int i);
int dice(int n, int s);
int dice2(const char* code);
void msg(const char *msg);
int expForLevel(int i);
int hpIncrForLevel(int i);
int player_act(THING* sprite);
int sprite_act(THING* sprite);
bool isFree(chtype ch);
THING* locateThing(int ypos, int xpos);
THING* locateObject(int ypos, int xpos);
THING* locateSprite(int ypos, int xpos);
void drinkEffect(int i);
void dumpInventory(int i);
THING* place(THING* thing);
int addToInventory(THING* t);
void pickUpObject(THING* sprite);
THING* placeAt(THING* thing, int y, int x);
void attemptMove(THING* sprite, int incrY, int incrX);
void combat(THING* thing, int atY, int atX);
WINDOW* newPopup(int lines);
chtype endPopup(WINDOW* win);

#endif
