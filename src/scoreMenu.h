#define SCORES_SCREEN_H

#include "raylib.h"
#include "player.h"
#include <stdbool.h>

bool DrawScoresScreen(const int *scores, int scoreCount, Player *p1,Player *p2);