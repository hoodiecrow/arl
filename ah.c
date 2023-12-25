#include "main.h"
#include "ah.h"

ahptr actionHandlers[256] = {
   /*0x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*1x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*2x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*3x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*4x*/&ah__, &ah__, &ah__, &ah__, &ah_p, &ah__, &ah_d, &ah_I, &ah__, &ah_1,
   /*5x*/&ah_2, &ah_3, &ah_4, &ah_5, &ah_6, &ah_7, &ah_8, &ah_9, &ah__, &ah__,
   /*6x*/&ah__, &ah__, &ah__, &ah_h, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_E, 
   /*7x*/&ah__, &ah__, &ah__, &ah_I, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*8x*/&ah_P, &ah_Q, &ah_R, &ah_S, &ah_T, &ah__, &ah__, &ah_W, &ah__, &ah__, 
   /*9x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_c, 
  /*10x*/&ah_d, &ah_e, &ah_r, &ah_g, &ah_h, &ah_i, &ah__, &ah__, &ah_l, &ah__, 
  /*11x*/&ah__, &ah__, &ah_p, &ah_q, &ah_r, &ah_s, &ah_t, &ah_u, &ah__, &ah_w, 
  /*12x*/&ah__, &ah__, &ah_z, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*13x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*14x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*15x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*16x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*17x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*18x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*19x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*20x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*21x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*22x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*23x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*24x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
  /*25x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__,
};



void ah__(THING* sprite) {
    (void)sprite;
    msg("unknown command");
    clrtoeol();
}

void ah_1(THING* sprite) {
    int maxy = getmaxy(sprite->room);
    if (sprite->ypos < maxy-2 && sprite->xpos > 1) {
        attemptMove(sprite, +1, -1);
    }
}

void ah_2(THING* sprite) {
    int maxy = getmaxy(sprite->room);
    if (sprite->ypos < maxy-2) {
        attemptMove(sprite, +1, +0);
    }
}

void ah_3(THING* sprite) {
    int maxy = getmaxy(sprite->room);
    int maxx = getmaxx(sprite->room);
    if (sprite->ypos < maxy-2 && sprite->xpos < maxx-2) {
        attemptMove(sprite, +1, +1);
    }
}

void ah_4(THING* sprite) {
    if (sprite->xpos > 1) {
        attemptMove(sprite, +0, -1);
    }
}

void ah_5(THING* sprite) {
    present(sprite);
}

void ah_6(THING* sprite) {
    int maxx = getmaxx(sprite->room);
    if (sprite->xpos < maxx-2) {
        attemptMove(sprite, +0, +1);
    }
}

void ah_7(THING* sprite) {
    if (sprite->ypos > 1 && sprite->xpos > 1) {
        attemptMove(sprite, -1, -1);
    }
}

void ah_8(THING* sprite) {
    if (sprite->ypos > 1) {
        attemptMove(sprite, -1, +0);
    }
}

void ah_9(THING* sprite) {
    int maxx = getmaxx(sprite->room);
    if (sprite->ypos > 1 && sprite->xpos < maxx-2) {
        attemptMove(sprite, -1, +1);
    }
}

void ah_c(THING* sprite) {
    (void)sprite;
    msg("close what?");
    clrtoeol();
}

void ah_e(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to eat:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == ':') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        //TODO eatEffect();
        dumpInventory(i);
    } else {
        msg("you can't eat that"); clrtoeol(); refresh();
    }
}

void ah_E(THING* sprite) {
    (void)sprite;
    //TODO
    msg("equip something");
}

void ah_f(THING* sprite) {
    (void)sprite;
    //TODO
    msg("forward until reach something");
    clrtoeol();
}

void ah_g(THING* sprite) {
    (void)sprite;
    msg("get what?");
    clrtoeol();
}

// help string apparatus copied from Rogue3.6.3 source.
// it was just too pretty to resist.
struct h_list {
    char h_ch;
    char *h_desc;
};

// edited from Rogue3.6.3
struct h_list helpstr[] = {
    { '?',	",h,F1\tprints help" },
    { '/',	"\tidentify object" },
    { 't',	"<dir>\tthrow something" },
    { 'f',	"<dir>\tforward until find something" },
    { 'p',	"<dir>\tzap a wand in a direction" },
    { 'z',	"\tzap a wand or staff" },
    { '>',	"\tgo down a staircase" },
    { 's',	"\tsearch for trap/secret door" },
    { '5',	"\trest for a while" },
    { 'i',	"\tinventory" },
    { 'q',	"\tquaff potion" },
    { 'r',	"\tread paper" },
    { 'e',	"\teat food" },
    { 'w',	"\twield a weapon" },
    { 'W',	"\twear armor" },
    { 'T',	"\ttake armor off" },
    { 'P',	"\tput on ring" },
    { 'R',	"\tremove ring" },
    { 'd',	"\tdrop object" },
    { 'S',	"\tsave game" },
    { 'Q',	"\tquit" },
    { 0, 0 }
};

void ah_h(THING* sprite) {
    // show a single help line, or all of them
    // adapted from the code in Rogue 3.6.3
    (void)sprite;
    register struct h_list *strp = helpstr;
    msg("Character you want help for (* for all): ");
    chtype ch = getch();
    if (ch != '*') {
        move(0, 0);
        while (strp->h_ch) {
            if (strp->h_ch == (char)ch) {
                printw("%c%s", strp->h_ch, strp->h_desc);
                clrtoeol();
                refresh();
                break;
            }
            strp++;
        }
        if (strp->h_ch != (char)ch)
            printw("Unknown character '%c'", ch);
        return;
    }
    WINDOW* hw = create_newwin(22, 30, 2, 72);
    wclear(hw);
    int cnt = 0;
    while (strp->h_ch) {
        mvwaddch(hw, cnt, 0, strp->h_ch);
        waddstr(hw, strp->h_desc);
        cnt++;
        strp++;
    }
    wrefresh(hw);
    ch = wgetch(hw);
    werase(hw);
    destroy_win(hw);
    move(0, 0);
    clrtoeol();
}

void ah_i(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(15+3);
    mvwprintw(invlist, 1, 1, "%s", "You are carrying:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
        if (inventory[i] == wielded)
            waddstr(invlist, " (wielded)");
        if (inventory[i] == worn)
            waddstr(invlist, " (worn)");
        if (inventory[i] == right)
            waddstr(invlist, " (on right hand)");
        if (inventory[i] == left)
            waddstr(invlist, " (on left hand)");
    }
    endPopup(invlist);
}

void ah_l(THING* sprite) {
    (void)sprite;
    // 1st pass: count the items
    int itemcount = 0;
    for (THING* t = things; t != NULL; t = t->next) {
        if (t->glyph != '@' && !t->inInventory && t != worn)
            itemcount++;
    }
    WINDOW* invlist = newPopup(inventoryFill+3);
    // 2nd pass: display the items
    int i = 1;
    mvwprintw(invlist, i++, 1, "%s", "Looking around, you see:");
    for (THING* t = things; t != NULL; t = t->next) {
        if (t->glyph != '@' && !t->inInventory && t != worn) {
            mvwprintw(invlist, i++, 1, "%s", t->descr);
        }
    }
    endPopup(invlist);
}

void ah_q(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to drink:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '!') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        drinkEffect(i);
        dumpInventory(i);
    } else {
        msg("you can't drink that"); clrtoeol(); refresh();
    }
}

void ah_r(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to read:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '?') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        readEffect(i);
        dumpInventory(i);
    } else {
        msg("you can't read that"); clrtoeol(); refresh();
    }
}

void ah_s(THING* sprite) {
    (void)sprite;
    //TODO search for hidden things, using sprite for position
    msg("search hidden");
    clrtoeol();
}

void ah_t(THING* sprite) {
    (void)sprite;
    //TODO throw something
    msg("throw something");
    clrtoeol();
}

/* adapted from Rogue3.6.3 */
void ah_I(THING* sprite) {
    (void)sprite;
    //TODO
    char ch;
    const char* str;
    msg("What do you want identified? ");
    ch = getch();
    if (ch == 27) {
        msg("");
        return;
    }
    if (isupper(ch))
        str = monsters[ch-'A'].name;
    else
        switch(ch) {
            case '#':
                str = "wall of a room";
                break;
            case '*':
                str = "gold";
                break;
            case '>':
                str = "passage leading down";
                break;
            case '+':
                str = "door";
                break;
            case ' ':
                str = "room or corridor floor";
                break;
            case '@':
                str = "you";
                break;
            case '^':
                str = "trap";
                break;
            case '!':
                str = "potion";
                break;
            case '?':
                str = "scroll";
                break;
            case ':':
                str = "food";
                break;
            case ')':
                str = "weapon";
                break;
            case ']':
                str = "armour";
                break;
            case '=':
	            str = "ring";
                break;
            case '/':
                str = "wand";
                break;
            case '\\':
                str = "staff";
                break;
	        default:
                str = "unknown character";
    }
    mvprintw(0, 0, "'%c' : %s", ch, str);
    clrtoeol();
}

void ah_P(THING* sprite) {
    (void)sprite;
    //TODO put on a ring
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to put on:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '=') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        msg("wear on right hand (R) or left hand (L)?");
        switch (getch()) {
            case 'r': case 'R':
                if (right == NULL) {
                    right = inventory[i];
                    ringEffect(i);
                }
                break;
            case 'l': case 'L':
                if (left == NULL) {
                    left = inventory[i];
                    ringEffect(i);
                }
                break;
        }
    } else {
        msg("you can't put that on"); clrtoeol(); refresh();
    }
}

void ah_Q(THING* sprite) {
    (void)sprite;
    //TODO 
    msg("quit game");
    clrtoeol();
}

void ah_R(THING* sprite) {
    (void)sprite;
    //TODO remove a ring, if possible
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to equip:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '=' && (inventory[i] == right ||
                    inventory[i] == left)) {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        if (inventory[i]->isCursed) {
            msg("you can't");
            return;
        }
        if (inventory[i] == right)
            right = NULL;
        if (inventory[i] == left)
            left = NULL;
    }
}

void ah_S(THING* sprite) {
    (void)sprite;
    //TODO
    msg("save game");
    clrtoeol();
}

void ah_T(THING* sprite) {
    //TODO take off armour, if possible
    if (worn == NULL) {
        msg("you're not wearing armour");
        return;
    }
    if (worn->isCursed) {
        msg("you can't");
        return;
    }
    msg("you take your armour off");
    worn = NULL;
    sprite->armour = 10;
}

void ah_u(THING* sprite) {
    (void)sprite;
    msg("use what?");
    clrtoeol();
}

void ah_w(THING* sprite) {
    // wield a weapon
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to wield:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == ')') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        wieldEffect(i);
    } else {
        msg("you can't wield that"); clrtoeol(); refresh();
    }
}

void ah_W(THING* sprite) {
    (void)sprite;
    if (worn != NULL) {
        msg("you are already wearing armour");
        return;
    }
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to wear:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == ']') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        wearEffect(i);
    } else {
        msg("you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_z(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to zap with:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '\\' || inventory[i]->glyph == '/') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        // this is targetless zap
        zapEffect(i, false);
    } else {
        msg("you can't zap with that"); clrtoeol(); refresh();
    }
}

void ah_p(THING* sprite) {
    //TODO shares a lot of code with ah_z
    getDir(sprite->room);
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to zap with:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '\\' || inventory[i]->glyph == '/') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        // this is targetless zap
        zapEffect(i, true);
    } else {
        msg("you can't zap with that"); clrtoeol(); refresh();
    }
}

void ah_d(THING* sprite) {
    if (sprite->under != ' ') {
        msg("there is already something lying on this tile");
        return;
    }
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to drop:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (i >= 0 && i < inventoryFill) {
        THING* t = inventory[i];
        if (t == wielded) {
            if (wielded->isCursed) {
                msg("you can't");
                return;
            } else {
                unwield();
            }
        }
        if (t == worn) {
            msg("you must take that off first");
            return;
        }
        dumpInventory(i);
        sprite->under = t->glyph;
        t->ypos = sprite->ypos;
        t->xpos = sprite->xpos;
    }
}

