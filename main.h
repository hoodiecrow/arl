#ifndef arl_main_h
#define arl_main_h

#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
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
    struct THING* next;
    int attack;
    int constitution;
    int wholeConstitution;
    chtype under;
} THING;

enum POTIONS {
    BLINDNESS,
    CONFUSION,
    DETECT_MONSTER,
    DETECT_THINGS,
    EXTRA_HEALING,
    HALLUCINATION,
    HASTE_SELF,
    HEALING,
    INCREASE_STRENGTH,
    LEVITATION,
    POISON,
    RAISE_LEVEL,
    RESTORE_STRENGTH,
    SEE_INVISIBLE,
};

#define INVENTORY_SIZE 15

THING* newThing(WINDOW* win, ThingType type, chtype glyph, int y, int x);

THING* addMonster(WINDOW* win, const char* descr, int atk, int con);
THING* addArmour(WINDOW* win, const char* descr);
THING* addPotion(WINDOW* win);
THING* addGold(WINDOW* win);
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
