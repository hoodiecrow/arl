# arl
A roguelike

I have no plan and little knowledge, but I'm going to see how far I can get on just curiosity and a lot of googling.

Move around with keypad
```
7  8  9
4  5  6
1  2  3
```
(5 is rest)

# Actions
Around half of them do anything at all except show a text line, lol

- `c` close
- `e` **eat**
- `E` equip
- `g` get
- `i` **show inventory**
- `l` **look**
- `q` **drink**
- `r` **read**
- `u` use
- `w` **wear**
- `z` zap
- `,` **pick up**
- `.` **drop**
- `<` climb a stair
- `>` go down a stair
- `@` show character screen
- `CTRL-S` save game
- `F1` show help screen
- `Q` **quit**

# Objects
Still figuring these out...
- `!` potion or drink
- `"`
- `#` book
- `$` gold
- `%` carcass
- `&` garments or armour
- `'`
- `(` weapon
- `)` shield
- `*` food
- `+`
- `-`
- `/` wand
- `:` ring
- `;` amulet
- `<` stair (up)
- `=`
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

# Time management
To keep it simple for now, time management is Player-centric. The rest of the world acts when the player moves (including resting). Maybe some other actions will let the world act as well.

## My resources

- https://www.roguebasin.com/index.php/How_to_Write_a_Roguelike_in_15_Steps
- https://invisible-island.net/ncurses/man/ncurses.3x.html
- https://invisible-island.net/ncurses/NCURSES-Programming-HOWTO.html
- https://invisible-island.net/cdk/manpage/cdk.3.html
