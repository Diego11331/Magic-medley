#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "animations.h"
#include "player.h"
#include "gameManager.h"
#include "structures.h"
#include "wands.h"
#include "controlsMenu.h"
#include "mainMenu.h"
#include "scoreMenu.h"

#define SCREEN_W 1280
#define SCREEN_H 720
#define BRICK_WIDTH 32
#define BUTTON_COUNT 4
#define MAX_SCORE 1000

Structure structures[] = {
    // ===== SUELO =====
    {{0,         SCREEN_H-32, BRICK_WIDTH*10, 32}},
    {{448,       SCREEN_H-32, BRICK_WIDTH*12, 32}},
    {{960,       SCREEN_H-32, BRICK_WIDTH*10, 32}},

    // ===== PARED IZQUIERDA =====
    {{0,520,32,32}},
    {{0,552,32,32}},
    {{0,584,32,32}},
    {{0,616,32,32}},
    {{0,648,32,32}},

    // ===== PARED DERECHA =====
    {{SCREEN_W-32,520,32,32}},
    {{SCREEN_W-32,552,32,32}},
    {{SCREEN_W-32,584,32,32}},
    {{SCREEN_W-32,616,32,32}},
    {{SCREEN_W-32,648,32,32}},

    // ===== COBERTURAS BAJAS =====
    {{256, 624, BRICK_WIDTH*2, 32}},
    {{256, 656, BRICK_WIDTH*2, 32}},
    {{960, 624, BRICK_WIDTH*2, 32}},
    {{960, 656, BRICK_WIDTH*2, 32}},

    // ===== PRIMER NIVEL =====
    {{96,  560, BRICK_WIDTH*5, 32}},
    {{1024,560, BRICK_WIDTH*5, 32}},
    {{480, 520, BRICK_WIDTH*4, 32}},
    {{672, 520, BRICK_WIDTH*4, 32}},

    // ===== SEGUNDO NIVEL =====
    {{224, 430, BRICK_WIDTH*5, 32}},
    {{864, 430, BRICK_WIDTH*5, 32}},
    {{512, 390, BRICK_WIDTH*8, 32}},

    // ===== COBERTURA CENTRAL SEGUNDO NIVEL =====
    {{608, 422, BRICK_WIDTH*2, 32}},
    {{608, 454, BRICK_WIDTH*2, 32}},
    {{608, 486, BRICK_WIDTH*2, 32}},

    // ===== TERCER NIVEL =====
    {{96,  300, BRICK_WIDTH*4, 32}},
    {{1056,300, BRICK_WIDTH*4, 32}},
    {{384, 280, BRICK_WIDTH*4, 32}},
    {{768, 280, BRICK_WIDTH*4, 32}},

    // ===== COBERTURAS TERCER NIVEL =====
    {{320, 312, BRICK_WIDTH*2, 32}},
    {{320, 344, BRICK_WIDTH*2, 32}},
    {{896, 312, BRICK_WIDTH*2, 32}},
    {{896, 344, BRICK_WIDTH*2, 32}},

    // ===== CUARTO NIVEL =====
    {{256, 170, BRICK_WIDTH*4, 32}},
    {{896, 170, BRICK_WIDTH*4, 32}},
    {{544, 140, BRICK_WIDTH*6, 32}},

    // ===== COBERTURA SUPERIOR =====
    {{608, 172, BRICK_WIDTH*2, 32}},
    {{608, 204, BRICK_WIDTH*2, 32}},

    // ===== PLATAFORMAS EXTREMAS =====
    {{32,  200, BRICK_WIDTH*2, 32}},
    {{SCREEN_W-96, 200, BRICK_WIDTH*2, 32}},

    // ===== ZONA SUPERIOR NUEVA (simétrica) =====
    // Acceso desde plataformas extremas (~100px arriba = alcanzable con 1 salto)
    {{32,  100, BRICK_WIDTH*2, 32}},
    {{SCREEN_W-96, 100, BRICK_WIDTH*2, 32}},

    // Puentes intermedios simétricos (~90px arriba del cuarto nivel)
    {{192, 80,  32, 32}},   // misma altura que el puente
    {{192, 112, 32, 32}},   // bloque debajo → forma una L para cubrirse

    // Derecha — espejo exacto (1280 - 192 - 32 = 1056)
    {{1056, 80,  32, 32}},
    {{1056, 112, 32, 32}},

    // Plataforma central del legendario — más alta, requiere saltar desde los puentes
    // (~100px arriba de los puentes, alcanzable solo con doble salto desde ahí)
    {{544,  48, BRICK_WIDTH*6, 32}},
};

//Obtengo la cantidad de estructuras
int structuresLenght = sizeof(structures)/sizeof(structures[0]);

int main(void)
{
    //Estado inicial de la maquina
    GameState currentState=MENU;
    GameState prevState=MENU;

    InitWindow(SCREEN_W, SCREEN_H, "Magic medley");

    InitAudioDevice(); //Raylib llama a una libreria para reproducir sonidos

    //Cargo texturas de las estructuras
    Cloud clouds[MAX_CLOUDS]={0};
    Textures structsTextures=LoadTexturesStructures();
    InitClouds(clouds,structsTextures.cloud);

    //Inicio-Configuro los jugadores
    Player players[2]={0};
    InitPlayers(players);
    Player p1=players[0];
    Player p2=players[1];

    //Sprites de la ui del player
    Texture2D heart=LoadTexture("resources/Sprites/heart.png");
    Texture2D sword=LoadTexture("resources/Sprites/sword.png");
    
    //Sprites de las varitas
    WandTextures wandsTextures=LoadTexturesWands();

    //Animaicones jugadores
    Animation p1Anim={
        .first=0,
        .current=0,
        .last=2,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p1.png"),
        .spriteWidth=17,
        .spriteHeight=17
    };
    Animation p2IdleAnim={
        .first=0,
        .current=0,
        .last=3,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p2.png"),
        .spriteWidth=16,
        .spriteHeight=16
    };
    Animation p2RunAnim={
        .first=2,
        .current=2,
        .last=3,
        .speed=0.1f,
        .durationLeft=0.1f,
        .spriteSheet=LoadTexture("resources/Sprites/p2.png"),
        .spriteWidth=16,
        .spriteHeight=16
    };
    
    //Animaciones proyectiles

    Animation *proyectilesAnimations=LoadProyectilesAnimations();
    

    Projectile projectiles[MAX_PROJECTILES]={0};

    //Sistema de spawneo
    Wands worldWands[MAX_WORLD_WANDS]={0};
    WandSpawn *spawnRoot=InitSpawnTree();


    //======================Datos para el dibujado del menu
    Color bgColor=(Color){ 22, 18, 38, 255 };
    Color titleColor=(Color){ 235, 225, 255, 255 };
    Color subtitleColor=(Color){ 150, 135, 190, 255 };
 
    float btnWidth=300.0f;
    float btnHeight=58.0f;
    float gap=18.0f;
    float startY=400.0f;
    float centerX=SCREEN_W / 2.0f - btnWidth / 2.0f;
 
    MenuButton buttons[BUTTON_COUNT]={
        {(Rectangle){centerX,startY,btnWidth,btnHeight },"PLAY",BUTTON_PLAY,false,false},
        {(Rectangle){centerX,startY+(btnHeight+gap)*1,btnWidth, btnHeight },"CONTROLS",BUTTON_CONTROLS,false,false },
        {(Rectangle){centerX,startY+(btnHeight+gap)*2,btnWidth, btnHeight },"GLOBAL SCORE", BUTTON_SCORE,false,false },
        {(Rectangle){centerX,startY+(btnHeight+gap)*3,btnWidth, btnHeight },"EXIT",BUTTON_EXIT,false,false}
    };

    //Arreglo de scores
    int scores[MAX_SCORE]={0};

    //Para el global score
    int p1Score=0,p2Score=0;

    //Cargo los sonidos
    Sounds sounds=LoadSounds();
    Sound damageSound=LoadSound("resources/Sounds/hurtSound.mp3");

    //Cargo la musica
    Music menuMusic=LoadMusicStream("resources/Sounds/menuMusic.mp3");
    Music GameMusic=LoadMusicStream("resources/Sounds/gameMusic.mp3");

    menuMusic.looping=true;
    GameMusic.looping=true;

    PlayMusicStream(menuMusic);
    PlayMusicStream(GameMusic);

    //Textura del titulo
    Texture2D gameTitleTex=LoadTexture("resources/Sprites/gameTitle.png");

    //Timers para resetear el juego
    float resetTimerTime=2.0f,resetTimer=resetTimerTime;
    while (!WindowShouldClose())
    {
        //Actualizacion de variables condicional
        switch (currentState){
        case GAME:
            UpdatePlayer(&p1,GetFrameTime(),structures,structuresLenght);
            UpdatePlayer(&p2,GetFrameTime(),structures,structuresLenght);

            AnimationUpdate(&p1Anim);
            AnimationUpdate(&p2IdleAnim);
            AnimationUpdate(&p2RunAnim);

            //Actualizo nubes
            UpdateClouds(clouds);

            UpdateProjectile(projectiles,&p1,&p2,structures,structuresLenght,damageSound);

            UpdateWand(worldWands,&p1,&p2,(Vector2){25,-15},(Vector2){18,-13},4,projectiles);

            
            
            UpdateSpawnTree(spawnRoot,&p1,&p2,worldWands,proyectilesAnimations,wandsTextures,sounds);
            UpdateWandPickUp(&p1,&p2,worldWands);

            //Actualizo la musica del juego
             //Actualizo la musica del menu
            if(prevState==MENU){
                StopMusicStream(GameMusic);
                PlayMusicStream(GameMusic);
            }else{
                UpdateMusicStream(GameMusic);
            }
            break;
            case MENU: 
            case CONTROLS: 
            case SCORE: 
            //Actualizo la musica del menu
            if(prevState==GAME){
                StopMusicStream(menuMusic);
                PlayMusicStream(menuMusic);
            }else{
                UpdateMusicStream(menuMusic);
            }
            break;
        default:
            break;
        }
        //Tiene que estar afuera porque sino no recibe currentState acutalizado
        ResetLevel(&p1,&p2,SCREEN_H,(Vector2){128, 200},(Vector2){1000, 100},50,worldWands,spawnRoot,currentState,&p1Score,&p2Score,&resetTimer,&resetTimerTime);

        BeginDrawing();
        ClearBackground(bgColor);
        
        //Guardo el estado anterior
        prevState=currentState;

        //Dibujado condicional
        switch (currentState){
        case MENU:
            for(int i = 0; i < BUTTON_COUNT; i++) {
                bool isPrimary = (buttons[i].id == BUTTON_PLAY);
                bool clicked = DrawMenuButton(&buttons[i], isPrimary,titleColor,subtitleColor,gameTitleTex);
 
                if(clicked){
                    switch(buttons[i].id) {
                        case BUTTON_PLAY: currentState=GAME; break;
                        case BUTTON_CONTROLS: currentState=CONTROLS; break;
                        case BUTTON_SCORE: currentState=SCORE; break;
                        case BUTTON_EXIT: currentState=EXIT; break;
                    }
                }
            }
            break;
        case GAME:
            //Dibujando bkg y estructuras
            DrawStructures(structsTextures.brick,structsTextures.bkg,structsTextures.cloud,structuresLenght,structures,clouds);
            
            //UI
            DrawPlayerInfo(&p1,&p2,worldWands,heart,sword);

            //Dibujando jugadores
            DrawPlayerAnim(&p1Anim,3,p1.size,p1.position,p1Anim.spriteWidth,p1Anim.spriteHeight,-p1.direction,p1.damageTimer,p1.helth);
            if(p2.velocity.x!=0) DrawPlayerAnim(&p2RunAnim,4,p2.size,p2.position,p2RunAnim.spriteWidth,p2RunAnim.spriteHeight,p2.direction,p2.damageTimer,p2.helth);
            else DrawPlayerAnim(&p2IdleAnim,2,p2.size,p2.position,p2IdleAnim.spriteWidth,p2IdleAnim.spriteHeight,p2.direction,p2.damageTimer,p2.helth);

            //Dibujando varitas
            DrawWand(worldWands,&p1,&p2,wandsTextures);

            // DrawProjectiles(projectiles,&p1,&p2);
            DrawProjectiles(projectiles,&p1,&p2);

            //Para volver al menu
            if(IsKeyPressed(KEY_BACKSPACE)) currentState=MENU;
            break;
        case CONTROLS:
            if(DrawControlsScreen(&p1,&p2)) currentState=MENU;
        break;
        case SCORE:
            int scoreCount=ReadFileScore(scores,MAX_SCORE);
            if(DrawScoresScreen(scores,scoreCount,&p1,&p2)) currentState=MENU;
        break;
        case EXIT: 
            CloseWindow();
        break;
        default:
            break;
        }
            

        EndDrawing();
    }

    //No es necesario descargar las texturas, pero si tengo varios niveles, estas seguiran ocupando memoria ram
    // UnloadTexture(bkg);
    // UnloadTexture(brick);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(GameMusic);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
