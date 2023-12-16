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
    chtype badge;
    WINDOW* room;
    int ypos;
    int xpos;
    const char* descr;
    bool inInventory;
    bool isEdible;
    bool isPotable;
    bool isEquippable;
    struct THING* next;
    int attack;
    int constitution;
    chtype under;
} THING;

#define INVENTORY_SIZE 15

THING* newThing(WINDOW* win, ThingType type, chtype badge, int y, int x);

THING* addMonster(WINDOW* win, int y, int x, const char* descr, int atk, int con);
THING* addArmour(WINDOW* win, int y, int x, const char* descr);
void present(THING* thing);
int sprite_act(WINDOW* room, THING* sprite);
THING* locateThing(int ypos, int xpos);
THING* locateObject(int ypos, int xpos);
void dumpInventory(int i);
void attemptMove(WINDOW* room, THING* sprite, int incrY, int incrX);
void stepSprite(WINDOW* room, THING* thing, chtype floor, int toY, int toX);
void combat(THING* thing, int atY, int atX);
WINDOW* newPopup(int lines);
chtype endPopup(WINDOW* win);

#endif
