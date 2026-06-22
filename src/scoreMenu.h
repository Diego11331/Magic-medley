#include "raylib.h"
#include "player.h"
#include <stdbool.h>

int ReadFileScore(int *scores, int maxScores);
bool DrawScoresScreen(const int *scores, int scoreCount, Player *p1,Player *p2);