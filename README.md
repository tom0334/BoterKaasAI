# BoterKaasAI
A program that uses the minmax algorithm to beat human opponents in the games of tic tac toe and connect 4. It is unbeatable at the smaller board sizes since it considers all possible outcomes, but because of the exponential growth of possible boards, this becomes impossible at larger ones.

To combat that, the program has an evaluation function that determines the amount you have on a row, even if that is lower than you need to win. This means the program will always go for the move that makes it have the most N items on a row. Using this technique, it is so good at connect 4 that i and a few friends cannot beat it.

It supports boardsizes up to 5*5 with N on a row, and up to 7*7 with connect 4. It runs inside a terminal where it shows the board and asks you where you want to place your next item.


It was mostly written to gain some familiarity with C++. Have fun playing against it!
