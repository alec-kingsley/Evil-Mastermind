# Evil Mastermind (WIP)

A solver for a variant of Mastermind in which the Code Master is allowed to lie one time.

The lie can be changing the color of a peg, adding a peg, or removing a peg on any response to a
guess.

The main purpose of this project is for post-game analysis and experimentation with the variant,
although I may make it an interactive game at some point.

Currently, the program can show the best move and # of possibilities remaining after each guess.

## Results

This variant is surprisingly still solvable. Below is a game in which I gave the computer 15
guesses and allowed myself to lie 14 times, and the program was still able to solve it. (It knew
that I could lie 14 times, but did not know that I did lie on all of the first 14 moves)

Calculating with even one lie for humans, however, seems very difficult. 

```
┌─────┬────────────┐
│ B W │ 1  2  3  4 │
├─────┼────────────┤
│ 4 0 │ W  R  Y  B │
│ 3 0 │ W  R  Y  B │
│ 2 0 │ W  R  Y  R │
│ 0 1 │ B  Y  Y  P │
│ 0 2 │ B  B  W  Y │
│ 0 1 │ R  R  B  G │
│ 1 1 │ W  B  Y  G │
│ 0 1 │ P  R  P  Y │
│ 0 2 │ Y  B  W  P │
│ 1 0 │ W  R  G  P │
│ 1 2 │ Y  R  P  B │
│ 0 2 │ P  G  Y  W │
│ 0 2 │ R  W  W  Y │
│ 1 2 │ W  P  R  B │
│ 0 2 │ G  Y  B  R │
└─────┴────────────┘
```
  
## Running

Edit `main.c` to interact with the Evil Mastermind board and print data about it.

Then, build with `make` and run the resulting binary in `./bin/evil_mastermind`

## Configuring

`LIE_LIMIT` and `GUESS_LIMIT` can be configured in `board.h`. (A higher lie limit only applies to
the amount of turns you can lie on, you can never lie multiple times on the same turn)

`COLOR_CT` and `PEG_CT` can be configured in `guess.h`, although you may need to add more
colors and the ability to print them if `COLOR_CT` is modified.
