#include "raylib.h"
#define MAX_CLOUDS 8
#define BRICK_WIDTH 32

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
void DrawStructures(Texture2D brick,Texture2D bkg,Texture2D cloud,int structuresLenght,Structure *structures,Cloud clouds[]);
Textures LoadTexturesStructures();
void DrawClouds(Cloud clouds[]);
void InitClouds(Cloud clouds[], Texture2D cloudTexture);
void UpdateClouds(Cloud clouds[]);