#ifndef arl_ah_h
#define arl_ah_h

typedef void (*ahptr)(WINDOW* room, THING* sprite);

extern ahptr actionHandlers[];

void ah_1(WINDOW*, THING*); // 1 sw
void ah_2(WINDOW*, THING*); // 2 s
void ah_3(WINDOW*, THING*); // 3 se
void ah_4(WINDOW*, THING*); // 4 w
void ah_5(WINDOW*, THING*); // 5 rest
void ah_6(WINDOW*, THING*); // 6 e
void ah_7(WINDOW*, THING*); // 7 nw
void ah_8(WINDOW*, THING*); // 8 n
void ah_9(WINDOW*, THING*); // 9 ne
void ah_E(WINDOW*, THING*); // E equip
void ah_S(WINDOW*, THING*); // S up stair, also <
void ah__(WINDOW*, THING*); // _ null action
void ah_c(WINDOW*, THING*); // c close smth
void ah_d(WINDOW*, THING*); // d drop, also .
void ah_e(WINDOW*, THING*); // e eat
void ah_g(WINDOW*, THING*); // g get
void ah_h(WINDOW*, THING*); // h help screen, also ? and F1
void ah_i(WINDOW*, THING*); // i inventory
void ah_l(WINDOW*, THING*); // l look around (broken)
void ah_p(WINDOW*, THING*); // p pick up, also ,
void ah_q(WINDOW*, THING*); // q quaff
void ah_r(WINDOW*, THING*); // r read
void ah_s(WINDOW*, THING*); // s down stair, also >
void ah_u(WINDOW*, THING*); // u use
void ah_w(WINDOW*, THING*); // w wear
void ah_z(WINDOW*, THING*); // z zap
void ahCS(WINDOW*, THING*); // Ctrl-S: save game
                            // Q quit game

#endif
