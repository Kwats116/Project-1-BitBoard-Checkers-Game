#include <stdio.h>
#include <string.h>
#include "checkers.h"

int main(void) {
    GameState g;
    InitGame(&g);
    char line[128];

    printf("Bitboard Checkers - simple two-player\n");
    printf("Enter moves like: b6-a5 or type save filename / load filename / quit\n\n");

    while (1) {
        PrintBoard(&g);
        printf("Move> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        // trim newline
        size_t L = strlen(line);
        if (L && line[L-1] == '\n') line[L-1] = '\0';
        if (strncmp(line, "quit", 4) == 0) break;
        if (strncmp(line, "save ", 5) == 0) {
            char *fn = line + 5;
            if (SaveGame(&g, fn)) printf("Saved to %s\n", fn);
            else printf("Failed to save %s\n", fn);
            continue;
        }
        if (strncmp(line, "load ", 5) == 0) {
            char *fn = line + 5;
            if (LoadGame(&g, fn)) printf("Loaded %s\n", fn);
            else printf("Failed to load %s\n", fn);
            continue;
        }

        int from, to;
        if (!parse_move_str(line, &from, &to)) {
            printf("Could not parse move. Use format b6-a5\n");
            continue;
        }
        if (!TryMove(&g, from, to)) {
            printf("Illegal move.\n");
            continue;
        }
    }

    printf("Goodbye.\n");
    return 0;
}