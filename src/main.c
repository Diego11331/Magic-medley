#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

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

#define DASH_COLDAWN 2.0f //Tiempo de recarga del dash
#define MAX_DASH_DIST 200.0f //Distancia del dash

//Estructuras del jugador y los obstaculos
typedef struct Player{
    //Propiedades
    int helth;
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
    float lastTapTime;
    float lastDashTime;
    bool isDashing;
    float firstDashPos;
    //Animaciones
    
}Player;

typedef struct Sructure{
    Rectangle rect;
    Texture2D sprite;
}Structure;

//Variables obstaculo
const int brickWitdth=32;
Structure structures[]={
    {{80,SCREEN_H-32,brickWitdth*35,32}}, //BASE
    {{300,500,brickWitdth*10,32}},
    {{250,600,brickWitdth*10,32}},
    {{650,400,brickWitdth*10,32}}
};

//Obtengo la cantidad de estructuras
int structuresLenght = sizeof(structures)/sizeof(structures[0]);

void UpdatePlayer(Player *p, float dt);
int main(void)
{
    
    InitWindow(SCREEN_W, SCREEN_H, "Magic medley");

    //Preparando la textura del bkg
    Texture2D bkg=LoadTexture("resources/Sprites/bkg.png");
    Rectangle bgkSource={0,0,bkg.width,bkg.height};
    Rectangle bkgDest={0,0,SCREEN_W,SCREEN_H};

    //Preparando la textura de las plataformas
    Texture2D brick=LoadTexture("resources/Sprites/brick.png");
    Rectangle brickSource={0,0,brick.width,brick.height};

    //Configuro jugadores
        Player p1={
            .helth=50,
            .position=(Vector2){400,280},
            .size={40,50},
            .color=RED,
            .coyoteTimer=0,
            .keyLeft=KEY_A,
            .keyRight=KEY_D,
            .keyJump=KEY_W,
            .keyChange=KEY_E,
            .keyPick=KEY_R,
            .keyAttack=KEY_F,
            .lastTapTime=0.0f,
            .lastDashTime=0.0f,
            .jumps=2
        };
        Player p2={
            .helth=50,
            .position=(Vector2){600,280},
            .size={40,50},
            .color=BLUE,
            .coyoteTimer=0,
            .keyLeft=KEY_LEFT,
            .keyRight=KEY_RIGHT,
            .keyJump=KEY_UP,
            .keyChange=KEY_L,
            .keyPick=KEY_O,
            .keyAttack=KEY_P,
            .lastTapTime=0.0f,
            .lastDashTime=0.0f,
            .jumps=2
        };

    while (!WindowShouldClose())
    {
        float dt=GetFrameTime();
        UpdatePlayer(&p1,dt);
        UpdatePlayer(&p2,dt);


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
            DrawRectangle(p1.position.x,p1.position.y,p1.size.x,p1.size.y,p1.color);
            DrawRectangle(p2.position.x,p2.position.y,p2.size.x,p2.size.y,p2.color);

            
            // DrawText(TextFormat("X %.0f Y %.0f", p1.position.x, p1.position.y),10, 10, 20, BLACK);
        EndDrawing();
    }

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
    if(movingLeft) targetVel-=MOV_SPD;
    if(movingRight) targetVel+=MOV_SPD;

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
    float now=0.0f;
    const float doubleTapTime=0.25f;
    //=================Dash
    if(IsKeyPressed(p->keyLeft)){
        now=GetTime();

        if(now-p->lastDashTime>DASH_COLDAWN &&  now-p->lastTapTime<doubleTapTime){
            p->firstDashPos=p->position.x;
            p->velocity.x+=(p->velocity.x<0) ?-2000 :2000;
            p->lastDashTime=now;
            p->isDashing=true;
        }
        p->lastTapTime=now;
    }else if(IsKeyPressed(p->keyRight)){
        now=GetTime();

        if(now-p->lastDashTime>DASH_COLDAWN &&  now-p->lastTapTime<doubleTapTime){
            p->firstDashPos=p->position.x;
            p->velocity.x+=(p->velocity.x<0) ?-2000 :2000;
            p->lastDashTime=now;
            p->isDashing=true;
        }
        p->lastTapTime=now;
    }

    //Para parar el dash
    if(p->isDashing){
        if(fabsf(p->position.x-p->firstDashPos)>MAX_DASH_DIST){
            p->velocity.x=0;
            p->isDashing=false;
        }
        
    }
    
}