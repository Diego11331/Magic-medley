#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "structures.h"

#define SCREEN_W 1280
#define SCREEN_H 720
#define MAX_CLOUDS 8

//Para luego retornar las texturas
typedef struct Textures {
    Texture2D bkg;
    Texture2D brick;
    Texture2D cloud;
} Textures;
//Estructuras de los obstaculos
typedef struct Sructure{
    Rectangle rect;
}Structure;
typedef struct Cloud{
    Vector2 position;
    float speed;
    float scale;
    float alpha;
    Texture2D texture;
}Cloud;

Textures LoadTexturesStructures(){
    Textures textures={
        .brick=LoadTexture("resources/Sprites/brick.png"), //Preparando la textura de las plataformas
        .bkg=LoadTexture("resources/Sprites/colored_castle.png"), //Preparando la textura del bk
        .cloud=LoadTexture("resources/Sprites/cloud1.png") //Preparando la textura del cloud
    };
    SetTextureFilter(textures.bkg, TEXTURE_FILTER_BILINEAR); //Para que el dibujado escalado sea mejor hecho por la gpu
    return textures;
}
//===================Nubes
void InitClouds(Cloud clouds[], Texture2D cloudTexture){
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].position.x = GetRandomValue(0, SCREEN_W);
        clouds[i].position.y = GetRandomValue(20, SCREEN_H);
        clouds[i].speed      = GetRandomValue(100,300) / 10.0f;
        clouds[i].scale      = GetRandomValue(8, 20)  / 10.0f;
        clouds[i].alpha      = GetRandomValue(5, 9)   / 10.0f;
        clouds[i].texture    = cloudTexture;
    }
}
void UpdateClouds(Cloud clouds[]){
    for(int i=0;i<MAX_CLOUDS;i++){
        clouds[i].position.x+=clouds[i].speed*GetFrameTime();

        if(clouds[i].position.x>SCREEN_W) clouds[i].position.x=-clouds[i].texture.width-(GetRandomValue(50,100));
    }
}
void DrawClouds(Cloud clouds[]){
    for(int i=0;i<MAX_CLOUDS;i++){
        DrawTextureEx(clouds[i].texture,(Vector2)clouds[i].position,0,clouds[i].scale,Fade(WHITE,clouds[i].alpha));
    }
}

void DrawStructures(Texture2D brick,Texture2D bkg,Texture2D cloud,int structuresLenght,Structure *structures,Cloud clouds[]){
    //Dibujando bkg
    Rectangle bgkSource={0,0,bkg.width,bkg.height};
    Rectangle bkgDest={0,0,SCREEN_W,SCREEN_H};
    DrawTexturePro(bkg,bgkSource,bkgDest,(Vector2){0,0},0,WHITE);

    //Dibujando nubes
    DrawClouds(clouds);

    
    //Dibujando estucturas
            for(int i=0;i<structuresLenght;i++){
                float scale=.5f;
                float tileDrawWidth=brick.width*scale;
                float tiles=structures[i].rect.width/tileDrawWidth;
                
                for(int j=0;j<tiles;j++){
                    
                    DrawTextureEx(brick,(Vector2){structures[i].rect.x+j*tileDrawWidth,structures[i].rect.y},0,scale,WHITE);
                }
                
            }
}