#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "animations.h"

#define SCREEN_W 1280
#define SCREEN_H 720

#define GRAVITY 1800.0f //Aceleracion al subir
#define GRAVITY_FALL 3200.0f //Aceleracion al bajar
#define JUMP_SPD -620.0f //Velocidad inicial al saltar
#define JUMP_CUT -150.0f //Velocidad al soltar el boton de salto
#define MOV_SPD 280.0f //Velocidad de mov horizontal
#define ACCELERATION 2200.0f //Aceleracion horizontal
#define DECELERATION 2800.0f //Frenado horizontal

#define COYOTE_TIME 0.1f //Tiempo para saltar luego de caer de plataforma
#define JUMP_BUFFER_TIME 0.12f //Tiempo para saltar sin haber tocado la plataforma

#define DASH_COLDAWN 1.0f //Tiempo de recarga del dash
#define MAX_DASH_DIST 200.0f //Distancia del dash

//Estructuras del jugador y los obstaculos
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
}Player;

typedef struct Sructure{
    Rectangle rect;
}Structure;


//Variables obstaculo
const int brickWidth=32;
Structure structures[] = {

    // ===== SUELO =====

    {{0,    SCREEN_H-32, brickWidth*10, 32}},
    {{448,  SCREEN_H-32, brickWidth*12, 32}},
    {{960,  SCREEN_H-32, brickWidth*10, 32}},

    // ===== PAREDES EXTERNAS =====

    {{0,520,32,168}},
    {{SCREEN_W-32,520,32,168}},

    // ===== COBERTURAS BAJAS =====

    {{256,624,brickWidth*2,64}},
    {{960,624,brickWidth*2,64}},

    // ===== PRIMER NIVEL =====

    {{96,560,brickWidth*5,32}},
    {{1024,560,brickWidth*5,32}},

    {{480,520,brickWidth*4,32}},
    {{672,520,brickWidth*4,32}},

    // ===== SEGUNDO NIVEL =====

    {{224,430,brickWidth*5,32}},
    {{864,430,brickWidth*5,32}},

    {{512,390,brickWidth*8,32}},

    // ===== COBERTURA CENTRAL =====

    {{608,422,brickWidth*2,96}},

    // ===== TERCER NIVEL =====

    {{96,300,brickWidth*4,32}},
    {{1056,300,brickWidth*4,32}},

    {{384,280,brickWidth*4,32}},
    {{768,280,brickWidth*4,32}},

    // ===== COBERTURAS ALTAS =====

    {{320,312,brickWidth*2,64}},
    {{896,312,brickWidth*2,64}},

    // ===== CUARTO NIVEL =====

    {{256,170,brickWidth*4,32}},
    {{896,170,brickWidth*4,32}},

    {{544,140,brickWidth*6,32}},

    // ===== COBERTURA SUPERIOR =====

    {{608,172,brickWidth*2,64}},

    // ===== PLATAFORMAS EXTREMAS =====

    {{32,200,brickWidth*2,32}},
    {{SCREEN_W-96,200,brickWidth*2,32}}
};

//Obtengo la cantidad de estructuras
int structuresLenght = sizeof(structures)/sizeof(structures[0]);

void UpdatePlayer(Player *p, float dt);

int main(void)
{
    
    InitWindow(SCREEN_W, SCREEN_H, "Magic medley");

    //Preparando la textura del bkg
    Texture2D bkg=LoadTexture("resources/Sprites/colored_desert.png");
    Rectangle bgkSource={0,0,bkg.width,bkg.height};
    Rectangle bkgDest={0,0,SCREEN_W,SCREEN_H};

    //Preparando la textura de las plataformas
    Texture2D brick=LoadTexture("resources/Sprites/brick.png");
    Rectangle brickSource={0,0,brick.width,brick.height};

    //Configuro jugadores
        Player p1={
            .helth=50,
            .position=(Vector2){128, 500},
            .size={32,48},
            .color=RED,
            .coyoteTimer=0,
            .keyLeft=KEY_A,
            .keyRight=KEY_D,
            .keyJump=KEY_W,
            .keyChange=KEY_E,
            .keyPick=KEY_R,
            .keyAttack=KEY_F,
            .lastLeftTapTime=0.0f,
            .lastRightTapTime=0.0f,
            .lastDashTime=0.0f,
            .jumps=2,
            .direction=1
        };
        Player p2={
            .helth=50,
            .position=(Vector2){1200, 500},
            .size={32,48},
            .color=BLUE,
            .coyoteTimer=0,
            .keyLeft=KEY_LEFT,
            .keyRight=KEY_RIGHT,
            .keyJump=KEY_UP,
            .keyChange=KEY_L,
            .keyPick=KEY_O,
            .keyAttack=KEY_P,
            .lastLeftTapTime=0.0f,
            .lastRightTapTime=0.0f,
            .lastDashTime=0.0f,
            .jumps=2,
            .direction=1
        };
    
         //Animaicones jugadores
    Animation p1Anim={
        .first=0,
        .current=0,
        .last=2,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p1.png"),
        .spriteWidth=17
    };
    Animation p2IdleAnim={
        .first=0,
        .current=0,
        .last=3,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p2.png"),
        .spriteWidth=16
    };
    Animation p2RunAnim={
        .first=2,
        .current=2,
        .last=3,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p2.png"),
        .spriteWidth=16
    };

    while (!WindowShouldClose())
    {
        float dt=GetFrameTime();
        UpdatePlayer(&p1,dt);
        UpdatePlayer(&p2,dt);

        AnimationUpdate(&p1Anim);
        AnimationUpdate(&p2IdleAnim);
        AnimationUpdate(&p2RunAnim);
        BeginDrawing();

            ClearBackground(RAYWHITE);

            //Dibujando bkg
            DrawTexturePro(bkg,bgkSource,bkgDest,(Vector2){0,0},0,WHITE);

            //Dibujando estucturas
            for(int i=0;i<structuresLenght;i++){
                float scale=.5f;
                float tileDrawWidth=brick.width*scale;
                float tiles=structures[i].rect.width/tileDrawWidth;
                
                for(int j=0;j<tiles;j++){
                    
                    DrawTextureEx(brick,(Vector2){structures[i].rect.x+j*tileDrawWidth,structures[i].rect.y},0,scale,WHITE);
                }
                
            }

            //Dibujando jugadores
            // DrawRectangle(p1.position.x,p1.position.y,p1.size.x,p1.size.y,p1.color);
            // DrawRectangle(p2.position.x,p2.position.y,p2.size.x,p2.size.y,p2.color);

            DrawAnimation(&p1Anim,3,p1.size,p1.position,p1Anim.spriteWidth,-p1.direction);
            if(p2.velocity.x!=0) DrawAnimation(&p2RunAnim,4,p2.size,p2.position,p2RunAnim.spriteWidth,p2.direction);
            else DrawAnimation(&p2IdleAnim,2,p2.size,p2.position,p2IdleAnim.spriteWidth,p2.direction);
        EndDrawing();
    }

    //No es necesario descargar las texturas, pero si tengo varios niveles, estas seguiran ocupando memoria ram
    UnloadTexture(bkg);
    UnloadTexture(brick);
    CloseWindow();
    return 0;
}
void UpdatePlayer(Player *p,float dt){
    //Actualizando timers
    if(!p->onGround) p->coyoteTimer-=dt;
    if(p->jumpBufferTimer>0) p->jumpBufferTimer-=dt;

    //===========Movimiento horizontal
    bool movingLeft = IsKeyDown(p->keyLeft);
    bool movingRight = IsKeyDown(p->keyRight);

    float targetVel=0; //esta variable queda en 0 cada frame
    if(movingLeft){
        targetVel-=MOV_SPD;
        p->direction=-1;
    } 
    if(movingRight){
        targetVel+=MOV_SPD;
        p->direction=1;
    } 

    float accel = (targetVel!=0) ? ACCELERATION : DECELERATION; //Velocidad que va a aumentar o disminuir 

    float leftVelocity = targetVel-p->velocity.x; //Velocidad que falta para alcanzar la deseada

    float step = accel*dt; //Disminucion o aumento por frame

    if(step>fabsf(leftVelocity)) step=fabsf(leftVelocity); //Si el ultimo paso es mayor que lo que resta para alcanzar la velocidad deseada se iguala

    p->velocity.x+=(leftVelocity>0 ?1 :-1 )*step; //Se actualiza la velocidad

    //==============Salto
    bool jumpPressed=IsKeyPressed(p->keyJump);
    bool jumpReleased=IsKeyReleased(p->keyJump);
    p->jumpPressed=IsKeyPressed(p->keyJump);

    if(jumpPressed) p->jumpBufferTimer=JUMP_BUFFER_TIME;

    bool canJump=p->coyoteTimer>0; //Puedo saltar si no termino el contador

    if(p->onGround) p->jumps=2;

    //Ejecutar el salto
    if(p->jumpBufferTimer>0 && canJump){
        p->velocity.y=JUMP_SPD;
        p->jumpBufferTimer=0;
        p->coyoteTimer=0;
        p->jumps--;
    }else if(p->jumps>0 && jumpPressed){
        p->velocity.y=JUMP_SPD;
        p->jumps--;
    } 

    //Cortar el salto si se suelta el espacio
    if(jumpReleased && p->velocity.y<JUMP_CUT){
        p->velocity.y=JUMP_CUT;
    }

    //Aplicando gravedad a la velocidad en y
    float gravity=(p->velocity.y>0) ?GRAVITY_FALL :GRAVITY;
    p->velocity.y+=gravity*dt;

    //Velocidad maxima en y
    if(p->velocity.y>1200) p->velocity.y=1200;

    //=============Colisiones
    p->onGround=false;
    //Colisiones en y
    p->position.y+=p->velocity.y*dt; //Actualizo la pos en y

    Rectangle pRect={p->position.x,p->position.y,p->size.x,p->size.y}; //Actualizo el rectuangulo del jugador

    for(int i=0;i<structuresLenght;i++){
        if(CheckCollisionRecs(pRect,structures[i].rect)){
            if(p->velocity.y>0){ //Esta callendo
                p->position.y=structures[i].rect.y-p->size.y;
                p->onGround=true;
                p->velocity.y=0;
                p->coyoteTimer=COYOTE_TIME;
            }else if(p->velocity.y<0){ //Se choco de cabeza
                p->position.y=structures[i].rect.y+structures[i].rect.height;
                p->velocity.y=0;
            }
            pRect.y=p->position.y;
        }
    }
    //Colisiones en x
    p->position.x+=p->velocity.x*dt;
    pRect=(Rectangle){p->position.x,p->position.y,p->size.x,p->size.y};

    for(int i=0;i<structuresLenght;i++){
        if(CheckCollisionRecs(pRect,structures[i].rect)){
            if(p->velocity.x>0){ //Esta llendo hacia la derecha
                p->position.x=structures[i].rect.x-p->size.x;
                p->velocity.x=0;
            }else{ //Se choco en la izquierda
                p->position.x=structures[i].rect.x+structures[i].rect.width;
                p->velocity.x=0;
            }
            pRect.x=p->position.x;
        }
    }
    
    //=================Dash
    float now=0.0f;
    const float doubleTapTime=0.2f;
    if(IsKeyPressed(p->keyLeft)){
        now=GetTime();

        if(now-p->lastDashTime>DASH_COLDAWN &&  now-p->lastLeftTapTime<doubleTapTime){
            p->firstDashPos=p->position.x;
            p->velocity.x+=(p->velocity.x<0) ?-2000 :2000;
            p->lastDashTime=now;
            p->isDashing=true;
        }
        p->lastLeftTapTime=now;
    }else if(IsKeyPressed(p->keyRight)){
        now=GetTime();

        if(now-p->lastDashTime>DASH_COLDAWN &&  now-p->lastRightTapTime<doubleTapTime){
            p->firstDashPos=p->position.x;
            p->velocity.x+=(p->velocity.x<0) ?-2000 :2000;
            p->lastDashTime=now;
            p->isDashing=true;
        }
        p->lastRightTapTime=now;
    }

    //Para parar el dash
    if(p->isDashing){
        if(fabsf(p->position.x-p->firstDashPos)>MAX_DASH_DIST){
            p->velocity.x=0;
            p->isDashing=false;
        }
    }
    
}