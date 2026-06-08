#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
int main(void)
{
    const int screenWidth = 500;
    const int screenHeight = 500;
    
    float dt;
    float speed=200;
    Vector2 pos=(Vector2){screenWidth/2,screenHeight/2};
    Vector2 dir=(Vector2){0,0};
    
    Camera2D camera={0};
    camera.target=pos;
    camera.offset=(Vector2){screenWidth/2,screenHeight/2};
    camera.zoom=1.0f;
    
    InitWindow(screenWidth, screenHeight, "Game");

    while (!WindowShouldClose())
    {
        dir.x=(int)IsKeyDown(KEY_D)-(int)IsKeyDown(KEY_A);
        dir.y=(int)IsKeyDown(KEY_S)-(int)IsKeyDown(KEY_W);
        dir=Vector2Normalize(dir);
        
        dt=GetFrameTime();
        
        pos.x+=dir.x*speed*dt;
        pos.y+=dir.y*speed*dt;
        
        camera.target=pos;
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
            
            DrawCircle(pos.x, pos.y, 50, RED);
            DrawCircle(0, 0, 50, BLUE);

            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}