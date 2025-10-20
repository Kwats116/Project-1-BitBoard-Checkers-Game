#ifndef CHECKERS_H
#define CHECKERS_H

#include <stdint.h>

/* Bitboard mapping:
   We use a 64-bit unsigned where bit 0 is square a1 (lower-left from white's view)
   and bit 63 is h8 (top-right). The provided spec shows a specific mapping;
   adjust Index <-> algebraic helpers as needed. */

typedef struct {
    uint64_t red;      // red pieces (player 1)
    uint64_t black;    // black pieces (player 2)
    uint64_t red_k;    // red kings
    uint64_t black_k;  // black kings
    int turn;          // 0 = red to move, 1 = black to move
} GameState;

/* Create/initialize */
void InitGame(GameState *g);

/* Board helpers */
int rc_to_index(int r, int c);               // row 0..7, col 0..7 -> index 0..63
int algebraic_to_index(const char *s);       // "a1".."h8" -> index or -1
void index_to_algebraic(int idx, char *out); // fills 3-char string like "a1"

/* Queries */
int IsOccupied(const GameState *g, int idx);
int IsRed(const GameState *g, int idx);
int IsBlack(const GameState *g, int idx);
int IsKing(const GameState *g, int idx);

/* Moves: simple move and capture (single jump). This implementation
   will handle single-step moves and single captures for clarity.
   Multiple jumps can be added easily. */

/* Attempt a move e.g. from_idx -> to_idx; returns 1 if move applied */
int TryMove(GameState *g, int from_idx, int to_idx);

/* Save/load */
int SaveGame(const GameState *g, const char *filename);
int LoadGame(GameState *g, const char *filename);

/* Display */
void PrintBoard(const GameState *g);

/* Helpers for demo: parse move strings like "b6-a5" */
int parse_move_str(const char *s, int *from_idx, int *to_idx);

#endif // CHECKERS_H