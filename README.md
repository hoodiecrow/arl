# arl
A roguelike

I have no plan and little knowledge, but I'm going to see how far I can get on just curiosity and a lot of googling.

2023-12-21: Tomorrow it will be a week of working on this. State of the code: a mess. State of the game: just barely playable. Tomorrow I can claim a 7DRL (seven day roguelike challenge) and pack it up. I think I will keep at it for some time, though.

## Controls
Move around with keypad
```
7  8  9
4  5  6
1  2  3
```
(5 is rest)

## Actions
Bolded commands have some kind of mechanism to them.

- `c` close
- `e` **eat**
- `E` **equip**
- `g` get
- `i` **show inventory**
- `l` **look**
- `q` **drink**
- `r` **read**
- `u` use
- `w` **wield**
- `W` **wear**
- `z` zap
- `,` **pick up**
- `.` **drop**
- `<` climb a stair
- `>` go down a stair
- `@` show character screen
- `CTRL-S` save game
- `F1` show help screen
- `Q` **quit**

## Objects
Still figuring these out...
- `!` potion or drink
- `"`
- `$` gold
- `%` carcass
- `&` garments or armour
- `'`
- `(` 
- `)` weapon
- `*` food
- `+`
- `-`
- `/` wand
- `:` ring
- `;` amulet
- `<` stair (up)
- `=` book
- `>` stair (down)
- `?`
- `[`
- `\` staff
- `]`
- `^`
- `_`
- `{`
- `|` rod
- `}`
- `~` scroll

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
