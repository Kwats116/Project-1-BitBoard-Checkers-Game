#include "checkers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static inline uint64_t bit(int idx) { return (1ULL << idx); }

void InitGame(GameState *g) {
    /* Standard checkers initial positions on dark squares.
       For simplicity we put red on rows 0..2 and black on rows 5..7 (adjust as needed). */
    g->red = 0ULL;
    g->black = 0ULL;
    g->red_k = 0ULL;
    g->black_k = 0ULL;
    g->turn = 0;

    // mark red pieces on rows 0-2 on dark squares
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 == 1) {
                int idx = rc_to_index(r, c);
                g->red |= bit(idx);
            }
        }
    }
    // black on rows 5-7
    for (int r = 5; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 == 1) {
                int idx = rc_to_index(r, c);
                g->black |= bit(idx);
            }
        }
    }
}

/* row 0..7 from white's bottom (a1 is col=0,row=0) */
int rc_to_index(int r, int c) {
    if (r < 0 || r > 7 || c < 0 || c > 7) return -1;
    return r * 8 + c;
}

int algebraic_to_index(const char *s) {
    if (!s || strlen(s) < 2) return -1;
    char file = tolower(s[0]);
    char rank = s[1];
    if (file < 'a' || file > 'h') return -1;
    if (rank < '1' || rank > '8') return -1;
    int c = file - 'a';
    int r = rank - '1';
    return rc_to_index(r, c);
}

void index_to_algebraic(int idx, char *out) {
    int r = idx / 8;
    int c = idx % 8;
    out[0] = 'a' + c;
    out[1] = '1' + r;
    out[2] = '\0';
}

int IsOccupied(const GameState *g, int idx) {
    uint64_t mask = bit(idx);
    return ((g->red | g->black) & mask) ? 1 : 0;
}

int IsRed(const GameState *g, int idx) {
    return (g->red & bit(idx)) ? 1 : 0;
}
int IsBlack(const GameState *g, int idx) {
    return (g->black & bit(idx)) ? 1 : 0;
}
int IsKing(const GameState *g, int idx) {
    return ((g->red_k | g->black_k) & bit(idx)) ? 1 : 0;
}

static int in_bounds_rc(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }

/* helper to remove piece at idx */
static void clear_square(GameState *g, int idx) {
    uint64_t m = ~bit(idx);
    g->red &= m;
    g->black &= m;
    g->red_k &= m;
    g->black_k &= m;
}

static int is_red_turn(const GameState *g) { return g->turn == 0; }

/* Apply a move, not checking mandatory capture rules. Returns 1 on success. */
int TryMove(GameState *g, int from_idx, int to_idx) {
    if (from_idx < 0 || to_idx < 0 || from_idx >= 64 || to_idx >= 64) return 0;
    if (!IsOccupied(g, from_idx)) return 0;
    if (IsOccupied(g, to_idx)) return 0;

    int moving_red = IsRed(g, from_idx);
    int moving_black = IsBlack(g, from_idx);
    if (is_red_turn(g) && !moving_red) return 0;
    if (!is_red_turn(g) && !moving_black) return 0;

    int from_r = from_idx / 8, from_c = from_idx % 8;
    int to_r = to_idx / 8, to_c = to_idx % 8;
    int dr = to_r - from_r;
    int dc = to_c - from_c;

    int king = IsKing(g, from_idx);

    /* Regular move: diagonal by 1 in forward direction (or both for king) */
    int forward_dir = moving_red ? 1 : -1; // red moves up (increasing row)
    if ((abs(dr) == 1) && (abs(dc) == 1)) {
        if (!king) {
            if (dr != forward_dir) return 0; // cannot move backward if not king
        }
        // execute move
        uint64_t piece_mask = bit(from_idx);
        clear_square(g, from_idx);

        if (moving_red) g->red |= bit(to_idx);
        else g->black |= bit(to_idx);

        // preserve king status if already king
        if (king) {
            if (moving_red) g->red_k |= bit(to_idx);
            else g->black_k |= bit(to_idx);
        } else {
            // check promotion
            if (moving_red && to_r == 7) g->red_k |= bit(to_idx);
            if (moving_black && to_r == 0) g->black_k |= bit(to_idx);
        }

        g->turn = 1 - g->turn;
        return 1;
    }

    /* Capture: move by 2 in diagonal and jump over opponent */
    if ((abs(dr) == 2) && (abs(dc) == 2)) {
        int mid_r = (from_r + to_r) / 2;
        int mid_c = (from_c + to_c) / 2;
        int mid_idx = rc_to_index(mid_r, mid_c);

        if (mid_idx < 0) return 0;
        if (!(IsOccupied(g, mid_idx))) return 0;
        if (moving_red && IsRed(g, mid_idx)) return 0;
        if (moving_black && IsBlack(g, mid_idx)) return 0;

        if (!king) {
            if (dr != 2*forward_dir) return 0; // non-king must capture forward only
        }

        // execute capture
        clear_square(g, mid_idx); // remove captured piece
        clear_square(g, from_idx);

        if (moving_red) g->red |= bit(to_idx);
        else g->black |= bit(to_idx);

        if (king) {
            if (moving_red) g->red_k |= bit(to_idx);
            else g->black_k |= bit(to_idx);
        } else {
            // promotion
            if (moving_red && to_r == 7) g->red_k |= bit(to_idx);
            if (moving_black && to_r == 0) g->black_k |= bit(to_idx);
        }

        g->turn = 1 - g->turn;
        return 1;
    }

    return 0; // invalid move
}

/* Save as simple text file with four 64-bit hex values and turn */
int SaveGame(const GameState *g, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "%016llX\n%016llX\n%016llX\n%016llX\n%d\n",
            (unsigned long long)g->red,
            (unsigned long long)g->black,
            (unsigned long long)g->red_k,
            (unsigned long long)g->black_k,
            g->turn);
    fclose(f);
    return 1;
}

int LoadGame(GameState *g, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    unsigned long long a,b,c,d;
    int t;
    if (fscanf(f, "%llX\n%llX\n%llX\n%llX\n%d\n", &a, &b, &c, &d, &t) != 5) {
        fclose(f);
        return 0;
    }
    g->red = (uint64_t)a;
    g->black = (uint64_t)b;
    g->red_k = (uint64_t)c;
    g->black_k = (uint64_t)d;
    g->turn = t;
    fclose(f);
    return 1;
}

void PrintBoard(const GameState *g) {
    puts("    a b c d e f g h");
    puts("   -----------------");
    for (int r = 7; r >= 0; --r) {
        printf("%d | ", r+1);
        for (int c = 0; c < 8; ++c) {
            int idx = rc_to_index(r, c);
            char ch = '.';
            if (IsRed(g, idx)) ch = IsKing(g, idx) ? 'R' : 'r';
            else if (IsBlack(g, idx)) ch = IsKing(g, idx) ? 'B' : 'b';
            printf("%c ", ch);
        }
        printf("| %d\n", r+1);
    }
    puts("   -----------------");
    puts("    a b c d e f g h");
    printf("Turn: %s\n", g->turn == 0 ? "Red (r/R)" : "Black (b/B)");
}

/* parse "b6-a5" or "b6 a5" or "b6-a5\n" */
int parse_move_str(const char *s, int *from_idx, int *to_idx) {
    if (!s) return 0;
    char buf[64];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    // replace '-' with space
    for (char *p = buf; *p; ++p) if (*p == '-') *p = ' ';
    char a[8], b[8];
    if (sscanf(buf, "%7s %7s", a, b) != 2) return 0;
    int f = algebraic_to_index(a);
    int t = algebraic_to_index(b);
    if (f < 0 || t < 0) return 0;
    *from_idx = f;
    *to_idx = t;
    return 1;
}