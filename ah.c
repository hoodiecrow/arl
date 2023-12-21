#include "main.h"
#include "ah.h"

ahptr actionHandlers[256] = {
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ahCS, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah_p, &ah__, &ah_d, &ah__, &ah__, &ah_1,
    &ah_2, &ah_3, &ah_4, &ah_5, &ah_6, &ah_7, &ah_8, &ah_9, &ah__, &ah__,
    &ah_S, &ah__, &ah_s, &ah_h, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_E, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah_S, &ah__, &ah__, &ah__, &ah_W, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah_c, 
    &ah_d, &ah_e, &ah__, &ah_g, &ah_h, &ah_i, &ah__, &ah__, &ah_l, &ah__, 
    &ah__, &ah__, &ah_p, &ah_q, &ah_r, &ah_s, &ah__, &ah_u, &ah__, &ah_w, 
    &ah__, &ah__, &ah_z, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, &ah__, 
    &ah__, &ah__, &ah__, &ah__, &ah__, &ah__,
};



void ah__(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
}

void ahCS(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "save game");
    clrtoeol();
}

void ah_1(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    if (sprite->ypos < maxy-2 && sprite->xpos > 1) {
        attemptMove(room, sprite, +1, -1);
    }
}

void ah_2(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    if (sprite->ypos < maxy-2) {
        attemptMove(room, sprite, +1, +0);
    }
}

void ah_3(WINDOW* room, THING* sprite) {
    int maxy = getmaxy(room);
    int maxx = getmaxx(room);
    if (sprite->ypos < maxy-2 && sprite->xpos < maxx-2) {
        attemptMove(room, sprite, +1, +1);
    }
}

void ah_4(WINDOW* room, THING* sprite) {
    if (sprite->xpos > 1) {
        attemptMove(room, sprite, +0, -1);
    }
}

void ah_5(WINDOW* room, THING* sprite) {
    (void)room;
    present(sprite);
}

void ah_6(WINDOW* room, THING* sprite) {
    int maxx = getmaxx(room);
    if (sprite->xpos < maxx-2) {
        attemptMove(room, sprite, +0, +1);
    }
}

void ah_7(WINDOW* room, THING* sprite) {
    if (sprite->ypos > 1 && sprite->xpos > 1) {
        attemptMove(room, sprite, -1, -1);
    }
}

void ah_8(WINDOW* room, THING* sprite) {
    if (sprite->ypos > 1) {
        attemptMove(room, sprite, -1, +0);
    }
}

void ah_9(WINDOW* room, THING* sprite) {
    int maxx = getmaxx(room);
    if (sprite->ypos > 1 && sprite->xpos < maxx-2) {
        attemptMove(room, sprite, -1, +1);
    }
}

void ah_c(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "close what?");
    clrtoeol();
}

void ah_e(WINDOW* room, THING* sprite) {
    (void)room;
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
        mvaddstr(1, 0, "you can't eat that"); clrtoeol(); refresh();
    }
}

void ah_E(WINDOW* room, THING* sprite) {
    (void)room;
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
            mvaddstr(1, 0, "wear on right hand (R) or left hand (L)?");
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
        mvaddstr(1, 0, "you can't equip that"); clrtoeol(); refresh();
    }
}

void ah_g(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "get what?");
    clrtoeol();
}

void ah_h(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "help screen");
    clrtoeol();
}

void ah_i(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    WINDOW* invlist = newPopup(15+3);
    mvwprintw(invlist, 1, 1, "%s", "You are carrying:");
    for (int i = 0; i < inventoryFill; i++) {
        mvwprintw(invlist, i+2, 1, "%c) %s", i+'a', inventory[i]->descr);
    }
    endPopup(invlist);
}

void ah_l(WINDOW* room, THING* sprite) {
    (void)room;
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

void ah_q(WINDOW* room, THING* sprite) {
    (void)room;
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
        mvaddstr(1, 0, "you can't drink that"); clrtoeol(); refresh();
    }
}

void ah_r(WINDOW* room, THING* sprite) {
    (void)room;
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
        mvaddstr(1, 0, "you can't read that"); clrtoeol(); refresh();
    }
}

void ah_s(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "you went down a stair");
    clrtoeol();
}

void ah_S(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "you went up a stair");
    clrtoeol();
}

void ah_u(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    mvaddstr(1, 0, "use what?");
    clrtoeol();
}

void ah_w(WINDOW* room, THING* sprite) {
    // wield a weapon
    (void)room;
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
        mvaddstr(1, 0, "you can't wield that"); clrtoeol(); refresh();
    }
}

void ah_W(WINDOW* room, THING* sprite) {
    (void)room;
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
        mvaddstr(1, 0, "you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_z(WINDOW* room, THING* sprite) {
    (void)room;
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
        mvaddstr(1, 0, "you can't wear that"); clrtoeol(); refresh();
    }
}

void ah_p(WINDOW* room, THING* sprite) {
    (void)room;
    (void)sprite;
    if (sprite->under == ' ') {
        mvaddstr(1, 0, "there's nothing to pick up");
    } else if (inventoryFill == INVENTORY_SIZE) {
        mvaddstr(1, 0, "you can't carry more items");
    } else {
        THING* t = locateObject(sprite->ypos, sprite->xpos);
        if (t == NULL) {
            mvaddstr(1, 0, "there's nothing to pick up");
        } else if (t->type == T_Structure) {
            mvaddstr(1, 0, "you can't pick that up");
        } else {
            inventory[inventoryFill++] = t;
mvprintw(1, 0, "you picked up: %s", t->descr);
            //TODO multiple items in same space
            sprite->under = ' ';
            t->inInventory = true;
            t->ypos = -1;
            t->xpos = -1;
        }
    }
    clrtoeol();
}

void ah_d(WINDOW* room, THING* sprite) {
    (void)room;
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

