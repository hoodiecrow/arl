#include "main.h"
#include "ah.h"

ahptr actionHandlers[256] = {
   /*0x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*1x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ahCS, 
   /*2x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*3x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*4x*/&ah__, &ah__, &ah__, &ah__, &ah_p, &ah__, &ah_d, &ah__, &ah__, &ah_1,
   /*5x*/&ah_2, &ah_3, &ah_4, &ah_5, &ah_6, &ah_7, &ah_8, &ah_9, &ah__, &ah__,
   /*6x*/&ah_S, &ah__, &ah_s, &ah_h, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_E, 
   /*7x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
   /*8x*/&ah__, &ah__, &ah__, &ah_S, &ah__, &ah__, &ah__, &ah_W, &ah__, &ah__, 
   /*9x*/&ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_c, 
  /*10x*/&ah_d, &ah_e, &ah__, &ah_g, &ah_h, &ah_i, &ah__, &ah__, &ah_l, &ah__, 
  /*11x*/&ah__, &ah__, &ah_p, &ah_q, &ah_r, &ah_s, &ah__, &ah_u, &ah__, &ah_w, 
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
}

void ahCS(THING* sprite) {
    (void)sprite;
    msg("save game");
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
        if (inventory[i]->isEdible) {
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
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to equip:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->isEquippable) {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        if (inventory[i]->glyph == ':') {
            msg("wear on right hand (R) or left hand (L)?");
            clrtoeol();
            refresh();
            switch (getch()) {
                case 'r': case 'R':
                    if (right != NULL) {
                        right->inInventory = true;
                    }
                    right = inventory[i];
                    equipEffect(i);
                    dumpInventory(i);
                    break;
                case 'l': case 'L':
                    if (left != NULL) {
                        left->inInventory = true;
                    }
                    left = inventory[i];
                    equipEffect(i);
                    dumpInventory(i);
                    break;
            }
        }
    } else {
        msg("you can't equip that"); clrtoeol(); refresh();
    }
}

void ah_g(THING* sprite) {
    (void)sprite;
    msg("get what?");
    clrtoeol();
}

void ah_h(THING* sprite) {
    (void)sprite;
    msg("help screen");
    clrtoeol();
}

void ah_i(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(15+3);
    mvwprintw(invlist, 1, 1, "%s", "You are carrying:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
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
        if (inventory[i]->isPotable) {
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
        if ((inventory[i]->glyph == '~' || inventory[i]->glyph == '=')) {
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
    msg("you went down a stair");
    clrtoeol();
}

void ah_S(THING* sprite) {
    (void)sprite;
    msg("you went up a stair");
    clrtoeol();
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
        if (wielded != NULL) {
            wielded->inInventory = true;
        }
        wielded = inventory[i];
        wieldEffect(i);
        dumpInventory(i);
    } else {
        msg("you can't wield that"); clrtoeol(); refresh();
    }
}

void ah_W(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to wear:");
    for (int i = 0; i < inventoryFill; i++) {
        if (inventory[i]->glyph == '&') {
            mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
            allowedIndices[i] = true;
        } else {
            allowedIndices[i] = false;
        }
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (allowedIndices[i]) {
        if (worn != NULL) {
            sprite->under = worn->glyph;
            worn->ypos = sprite->ypos;
            worn->xpos = sprite->xpos;
        }
        worn = inventory[i];
        wearEffect(i);
        dumpInventory(i);
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
        // TODO find a target as well
        zapEffect(i);
    } else {
        msg("you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_p(THING* sprite) {
    (void)sprite;
    if (sprite->under == ' ') {
        msg("there's nothing to pick up");
    } else if (inventoryFill == INVENTORY_SIZE) {
        msg("you can't carry more items");
    } else {
        THING* t = locateObject(sprite->ypos, sprite->xpos);
        if (t == NULL) {
            msg("there's nothing to pick up");
        } else if (t->type == T_Structure) {
            msg("you can't pick that up");
        } else {
            inventory[inventoryFill++] = t;
            mvprintw(0, 0, "you picked up: %s", t->descr);
            //TODO multiple items in same space
            sprite->under = ' ';
            t->inInventory = true;
            t->ypos = -1;
            t->xpos = -1;
        }
    }
    clrtoeol();
}

void ah_d(THING* sprite) {
    (void)sprite;
    WINDOW* invlist = newPopup(inventoryFill+3);
    mvwprintw(invlist, 1, 1, "%s", "What do you want to drop:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
    }
    chtype ch = endPopup(invlist);
    int i = ch-'a';
    if (i >= 0 && i < inventoryFill) {
        THING* t = inventory[i];
        dumpInventory(i);
        sprite->under = t->glyph;
        t->ypos = sprite->ypos;
        t->xpos = sprite->xpos;
    }
}

