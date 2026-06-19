#include "player.h"
#include "wands.h"


void ResetTimerSapwns(WandSpawn *root);
void ResetLevel(Player *p1,Player *p2,int screenHeight,Vector2 p1SpawnPos,Vector2 p2SpawnPos,float initialHelth,Wands wands[],WandSpawn *root);
void DrawPlayerInfo(Player *p1,Player *p2,Wands wands[],Texture2D heartTex,Texture2D swordTex);