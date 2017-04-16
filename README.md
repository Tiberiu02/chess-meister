# chess-meister
Chess AI written in C++

It reads the table from __table.in__, computes a move using a heavily optimized version of minimax with a depth of 7, then writes the new table to __table.out__

## Table fromat
A table is a matrix of 8 rows and 8 collumns, the rows are separated by _'\n'_-s and the elements on a row are separated by spaces. A matrix cell consists in 2 characters. The second stays for who owns the piece in that cell('0' for white or '1' for black) while the first stays for what piece is in that cell:
- 'P' for a Pawn
- 'B' for a Bishop
- 'N' for a Knight
- 'R' for a Rook
- 'K' for the King
- 'Q' for the Queen
- '\_' for an empty cell

Here is the initial table for clarification:  
`R1 N1 B1 Q1 K1 B1 N1 R1 `  
`P1 P1 P1 P1 P1 P1 P1 P1 `  
`_0 _0 _0 _0 _0 _0 _0 _0 `  
`_0 _0 _0 _0 _0 _0 _0 _0 `  
`_0 _0 _0 _0 _0 _0 _0 _0 `  
`_0 _0 _0 _0 _0 _0 _0 _0 `  
`P0 P0 P0 P0 P0 P0 P0 P0 `  
`R0 N0 B0 Q0 K0 B0 N0 R0 `

## Implementation
As said, the program is about minimax with some optimizations. Thanks to this awesome website:  
[https://chessprogramming.wikispaces.com/](https://chessprogramming.wikispaces.com/)

Here are some of the features:
- negamax
- alpha-beta pruning
- futility and deep futility pruning
- bitboards
- positional score

## Demo
An interactive demo in available here: [tiberiu.info/chess](http://tiberiu.info/chess)  

_Please note that the demo is not made to be used by more than one user at a time, so your table may be replaced by the one of another player at any time. Sorry!_

## To do
Any contributions are welcome. Here are a few _to do_'s:
- implement castling
- ~~make an entirely javascript version to run in the browser~~ tried and it was 1000 times slower
