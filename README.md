# arl
A roguelike

I have no plan and little knowledge, but I'm going to see how far I can get on just curiosity and a lot of googling.

2023-12-21: Tomorrow it will be a week of working on this. State of the code: a mess. State of the game: just barely playable. Tomorrow I can claim a 7DRL (seven day roguelike challenge) and pack it up. I think I will keep at it for some time, though.

## Objects
In playtesting the original Rogue for comparison, differences in symbols kept tripping me up. Solution: set my symbol set to that of Rogue (almost completely).
- `!` potion or drink
- `*` gold
- `]` armour
- `)` weapon
- `:` food
- `/` wand
- `=` ring
- `\` staff
- `?` scroll
- `<` stair (up)
- `>` stair (down)
- `_` carcass


## Controls
Just like with symbols, command keys were hard to keep up with and I decided to use Rogue keys.

Bolded commands have some kind of mechanism to them.

- `1` **sw**
- `2` **s**
- `3` **se**
- `4` **w**
- `5` **rest**
- `6` **e**
- `7` **nw**
- `8` **n**
- `9` **ne**
- `E` **equip***
- `I` identify object (also /)
- `P` put on ring
- `Q` **quit game**
- `R` remove ring
- `S` save game
- `T` take off armour
- `c` close smth*
- `d` **drop, also .**
- `e` eat
- `f` forward until reach something
- `g` get*
- `h` **help screen, also ? and F1**
- `i` **inventory**
- `l` **look around (broken)** *
- `p` **zap in a given direction**
- `q` **quaff**
- `r` **read**
- `s` search for hidden things
- `t` throw something
- `u` use*
- `w` **wield**
- `W` **wear**
- `z` **zap**
- `<` climb a stair*
- `>` go down a stair
- 
*) not in Rogue, may be removed


## Time management
To keep it simple for now, time management is Player-centric. The rest of the world acts when the player moves (including resting). Maybe some other actions will let the world act as well.

## Enemies
Move randomly unless near the player, in which case they attempt to approach and attack.

## Steps

1. ✓ 2. ✓ 3. ✓ 4. ✓ 5. - 6. ✓ 7. ✓ 8. - 9. ✓ 10. 
## My resources

- https://www.roguebasin.com/index.php/How_to_Write_a_Roguelike_in_15_Steps
- https://invisible-island.net/ncurses/man/ncurses.3x.html
- https://invisible-island.net/ncurses/NCURSES-Programming-HOWTO.html
- https://invisible-island.net/cdk/manpage/cdk.3.html
- https://www.roguebasin.com/index.php/Roguelike_Dev_FAQ
- http://roguelike.pbworks.com/w/page/7833587/Rogue
- https://nethackwiki.com/wiki/Rogue_(game)
