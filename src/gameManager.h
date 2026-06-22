#include "player.h"
#include "wands.h"

typedef enum{
    MENU,
    GAME,
    CONTROLS,
    SCORE,
    EXIT
}GameState;

void ResetTimerSapwns(WandSpawn *root);
void ResetLevel(Player *p1,Player *p2,int screenHeight,Vector2 p1SpawnPos,Vector2 p2SpawnPos,float initialHelth,Wands wands[],WandSpawn *root,GameState currentState,int *p1Score,int *p2Score);
void DrawPlayerInfo(Player *p1,Player *p2,Wands wands[],Texture2D heartTex,Texture2D swordTex);
bool WriteFileScore(int p1Score,int p2Score);