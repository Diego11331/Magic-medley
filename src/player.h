#include "raylib.h"
#include "raymath.h"
#include "structures.h"
#include "animations.h"


#define GRAVITY 1800.0f //Aceleracion al subir
#define GRAVITY_FALL 3200.0f //Aceleracion al bajar
#define JUMP_SPD -620.0f //Velocidad inicial al saltar
#define JUMP_CUT -150.0f //Velocidad al soltar el boton de salto
#define ACCELERATION 2200.0f //Aceleracion horizontal
#define DECELERATION 2800.0f //Frenado horizontal

#define COYOTE_TIME 0.1f //Tiempo para saltar luego de caer de plataforma
#define JUMP_BUFFER_TIME 0.12f //Tiempo para saltar sin haber tocado la plataforma

#define DASH_COLDAWN 1.0f //Tiempo de recarga del dash
#define MAX_DASH_DIST 200.0f //Distancia del dash

#define MAX_WANDS 2

typedef struct Player{
    //Propiedades
    int helth;
    int direction;
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
    Color color;
    //Manejar saltos
    float coyoteTimer;
    float jumpBufferTimer;
    int jumps;
    bool onGround;
    bool jumpPressed;
    //Controles
    int keyLeft,keyRight,keyJump,keyPick,keyChange,keyAttack;
    //Manejar dash
    float lastLeftTapTime;
    float lastRightTapTime;
    float lastDashTime;
    bool isDashing;
    float firstDashPos;
    //Inventario
    bool hasWand;
    int wandIdentifier[MAX_WANDS];
    //Efecto de dano
    float damageTimer;
    float damageTime;
    //Invertir controles
    float invertControlsTimer;
    float invertControlsTimerTime;
    //Esta conjelado
    float isFreezedTimer;
    float isFreezedTimerTime;
}Player;
void UpdatePlayer(Player *p,float dt,Structure structures[],int structuresLenght);
void InitPlayers(Player players[2]);
void DrawPlayerAnim(Animation *self,int numFramesPerRow,Vector2 pSize,Vector2 pPos,int spriteWidth,int spriteHeight,int pDirection,float damageTimer);