# Project-1-BitBoard-Checkers-Game

A bitboard-based Checkers engine written in C which Utilizes 64-bit bitboards.

# Gameplay rules-
Usual Checkers rules:
1) Movement is diaginal
2) To capture a peice, make sure there is a open spot diaginal of it
3) Moving a peice up all the way to the opposing side will turn it into a king, allowing it to move both Side to side as well as Diaginal

Build & run instructions instructions:

gcc -std=c11 -O2 -o checkers main.c bitops.c checkers.c

.\checkers.exe
