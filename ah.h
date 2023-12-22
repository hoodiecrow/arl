#ifndef arl_ah_h
#define arl_ah_h

typedef void (*ahptr)(THING* sprite);

extern ahptr actionHandlers[];

void ah_1(THING*); // 1 sw
void ah_2(THING*); // 2 s
void ah_3(THING*); // 3 se
void ah_4(THING*); // 4 w
void ah_5(THING*); // 5 rest
void ah_6(THING*); // 6 e
void ah_7(THING*); // 7 nw
void ah_8(THING*); // 8 n
void ah_9(THING*); // 9 ne
void ah_E(THING*); // E equip
void ah_S(THING*); // S up stair, also <
void ah__(THING*); // _ null action
void ah_c(THING*); // c close smth
void ah_d(THING*); // d drop, also .
void ah_e(THING*); // e eat
void ah_g(THING*); // g get
void ah_h(THING*); // h help screen, also ? and F1
void ah_i(THING*); // i inventory
void ah_l(THING*); // l look around (broken)
void ah_p(THING*); // p pick up, also ,
void ah_q(THING*); // q quaff
void ah_r(THING*); // r read
void ah_s(THING*); // s down stair, also >
void ah_u(THING*); // u use
void ah_w(THING*); // w wield
void ah_W(THING*); // W wear
void ah_z(THING*); // z zap
void ahCS(THING*); // Ctrl-S: save game
                            // Q quit game

#endif
