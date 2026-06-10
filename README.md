# Evil Mastermind (WIP)

A solver for a variant of Mastermind in which the Code Master is allowed to lie one time.

The lie can be changing the color of a peg, adding a peg, or removing a peg on any response to a
guess.

The main purpose of this project is for post-game analysis, although I may make it an interactive
game at some point.

## Running

Edit `main.c` to interact with the Evil Mastermind board and print data about it.

Then, build with `make` and run the resulting binary in `./bin/evil_mastermind`

## Configuring

`LIE_LIMIT` and `GUESS_LIMIT` can be configured in `board.h`. (A higher lie limit only applies to
the amount of turns you can lie on, you can never lie multiple times on the same turn)

`COLOR_CT` and `PEG_CT` can be configured in `guess.h`, although you may need to add more
colors and the ability to print them if `COLOR_CT` is modified.
