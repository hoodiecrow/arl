#ifndef arl_main_h
#define arl_main_h

#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	S_GOLD_DET,
	S_TELEPORT,
	S_ENCH_WPN,
	S_CREATE_M,
	S_REM_CURS,
	S_AGGR_MON,
	S_BLNK_PAP,
	NSCROLLS
};

enum RINGTYPES {
	R_ADORN,
	R_DEXT,
	R_MNT_ARM,
	R_SEE_INVIS,
	R_TELEPORT,
	NRINGS
};

enum POTIONTYPES {
	P_BLIND,
	P_CONFUS,
	P_DET_MON,
	P_DET_THG,
	P_EXT_HEAL,
	P_HALLUC,
	P_HASTE,
	P_HEALING,
	P_INC_STR,
	P_LEVIT,
	P_POISON,
	P_RS_LVL,
	P_REST_STR,
	P_SEE_INVIS,
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
    const char* descr;
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
    bool isConfused;
    int confusionDuration;
    bool canConfuse;
    bool isHasted;
    int hasteDuration;
    bool isLevitating;
    int  levitationDuration;
    bool isAsleep;
    int sleepDuration;
    int healingCycle;
    int armour;
    struct THING* next;
    int attack;
    int expAward;
    const char* damage;
    int wplus;
    chtype under;
    STATS *stats;
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
extern const char* monsterNames[];

THING* newThing(WINDOW* win, ThingType type, chtype glyph, int y, int x);

THING* addMonster(WINDOW* win);
THING* addWeapon(WINDOW* win);
THING* addArmour(WINDOW* win);
THING* addWand(WINDOW* win);
THING* addStaff(WINDOW* win);
THING* addScroll(WINDOW* win);
THING* addRing(WINDOW* win);
THING* addPotion(WINDOW* win);
THING* addGold(WINDOW* win);
void wieldEffect(int i);
void zapEffect(int i);
void readEffect(int i);
void equipEffect(int i);
void wearEffect(int i);
void drinkEffect(int i);
void present(THING* thing);
int player_act(WINDOW* room, THING* sprite);
int sprite_act(WINDOW* room, THING* sprite);
THING* locateThing(int ypos, int xpos);
THING* locateObject(int ypos, int xpos);
void drinkEffect(int i);
void dumpInventory(int i);
void attemptMove(WINDOW* room, THING* sprite, int incrY, int incrX);
void stepSprite(WINDOW* room, THING* thing, chtype floor, int toY, int toX);
void combat(THING* thing, int atY, int atX);
WINDOW* newPopup(int lines);
chtype endPopup(WINDOW* win);

#endif
