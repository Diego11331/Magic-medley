#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define G 400
#define PLAYER_JUMP_SPD 350f
#define PLAYER_HOR_SPD 200f

//Estructuras del jugador y los obstaculos
typedef struct Player{
    int helth;
    float speed;
    bool canJump;
    Vector2 position;
}Player;
typedef struct Sructure{
    Rectangle rect;
    Color color;
}Structure;


int main(void)
{
    const int screenWidth = 500;
    const int screenHeight = 500;
    
    InitWindow(screenWidth, screenHeight, "Magic medley");

    //Variables player
    Player player;
    player.helth=50;
    player.position=(Vector2){400,280};
    player.speed=0;
    player.canJump=false;

    //Variables obstaculo
    Structure structures[]={
        {{0,0,1000,400},LIGHTGRAY},
        {{0,400,1000,200},GRAY},
        {{300,200,400,10},GRAY},
        {{250,300,100,10},GRAY},
        {{650,300,100,10},GRAY},
    };

    //Obtengo la cantidad de estructuras
    int structuresLenght = sizeof(structures)/sizeof(structures[0]);


    while (!WindowShouldClose())
    {
        float dt=GetFrameTime();

        BeginDrawing();
            ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}