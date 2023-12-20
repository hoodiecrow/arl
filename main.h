#ifndef arl_main_h
#define arl_main_h

#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    T_Sprite,
    T_Item,
    T_Structure,
} ThingType;

typedef struct THING {
    ThingType type;
    chtype glyph;
    WINDOW* room;
    int ypos;
    int xpos;
    const char* descr;
    const char* ident;
    int gold;
    bool inInventory;
    bool isEdible;
    bool isPotable;
    bool isEquippable;
    bool isIdentified;
    bool isAggressive;
    bool isDead;
    bool isConfused;
    int confusionDuration;
    int armour;
    struct THING* next;
    int attack;
    int currConstitution;
    int fullConstitution;
    int expAward;
    int exp;
    chtype under;
} THING;

typedef struct TILE {
	bool isFloor;
	bool isBearTrap;
	THING *item;
} TILE;

#define INVENTORY_SIZE 15

THING* newThing(WINDOW* win, ThingType type, chtype glyph, int y, int x);

THING* addMonster(WINDOW* win);
THING* addArmour(WINDOW* win);
THING* addWand(WINDOW* win);
THING* addStaff(WINDOW* win);
THING* addScroll(WINDOW* win);
THING* addRing(WINDOW* win);
THING* addPotion(WINDOW* win);
THING* addGold(WINDOW* win);
void zapEffect(int i);
void readEffect(int i);
void equipEffect(int i);
void wearEffect(int i);
void drinkEffect(int i);
void present(THING* thing);
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
