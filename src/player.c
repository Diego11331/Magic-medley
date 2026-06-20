#include "raylib.h"
#include "player.h"
#include "raymath.h"
#include "structures.h"
#include "animations.h"




void UpdatePlayer(Player *p,float dt,Structure structures[],int structuresLenght){
    float MOV_SPD=(!p->isFreezedTimer>0)?280.0f :100.0f;
    
    //Actualizando timers
    if(!p->onGround) p->coyoteTimer-=dt;
    if(p->jumpBufferTimer>0) p->jumpBufferTimer-=dt;
    if(p->damageTimer>0) p->damageTimer-=dt; //Para aplicar el efecto de dano

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

    //Re invertir controles
    if(p->invertControlsTimer>0){
        p->invertControlsTimer-=GetFrameTime();
    }else if(p->invertControlsTimer<0){
        KeyboardKey aux=p->keyLeft;
        p->keyLeft=p->keyRight;
        p->keyRight=aux;
        p->invertControlsTimer=0;
    }
    
    //Descongelar jugador
    if(p->isFreezedTimer>0){
        p->isFreezedTimer-=GetFrameTime();
    }else if(p->isFreezedTimer<0){
        p->isFreezedTimer=0;
    }
}
void InitPlayers(Player players[2]){
    //Configuro jugadores
    players[0]=(Player){
        .helth=50,
        .position=(Vector2){128, 200},
        .size={32,48},
        .color=RED,
        .coyoteTimer=0,
        .keyLeft=KEY_A,
        .keyRight=KEY_D,
        .keyJump=KEY_W,
        .keyPick=KEY_S,
        .keyAttack=KEY_F,
        .lastLeftTapTime=0.0f,
        .lastRightTapTime=0.0f,
        .lastDashTime=0.0f,
        .jumps=2,
        .direction=1,
        .wandIdentifier = {-1, -1},
        .damageTime=0.1f,
        .damageTimer=0.0f,
        .invertControlsTimer=0.0f,
        .invertControlsTimerTime=3.0f,
        .isFreezedTimer=0.0f,
        .isFreezedTimerTime=3.0f
    };
    players[1]=(Player){
        .helth=50,
        .position=(Vector2){1000, 100},
        .size={32,48},
        .color=BLUE,
        .coyoteTimer=0,
        .keyLeft=KEY_LEFT,
        .keyRight=KEY_RIGHT,
        .keyJump=KEY_UP,
        .keyPick=KEY_DOWN,
        .keyAttack=KEY_P,
        .lastLeftTapTime=0.0f,
        .lastRightTapTime=0.0f,
        .lastDashTime=0.0f,
        .jumps=2,
        .direction=1,
        .wandIdentifier = {-1, -1},
        .damageTime=0.1f,
        .damageTimer=0.0f,
        .invertControlsTimer=0.0f,
        .invertControlsTimerTime=3.0f,
        .isFreezedTimer=0.0f,
        .isFreezedTimerTime=3.0f
    };
}
void DrawPlayerAnim(Animation *self,int numFramesPerRow,Vector2 pSize,Vector2 pPos,int spriteWidth,int spriteHeight,int pDirection,float damageTimer){
    Color color =(damageTimer>0) ?RED :WHITE;
    DrawAnimation(self,numFramesPerRow,pSize,pPos,spriteWidth,spriteHeight,pDirection,color);
}