#include "raylib.h"
#include "player.h"
#include "structures.h"

#define MAX_PROJECTILES 100
#define MAX_WORLD_WANDS 100
#define MAX_SPAWNS 8

typedef struct Wands{
    float damage;
    float velocity;
    Vector2 spawnPos;
    Vector2 position;
    Vector2 size;
    float projectileSize;
    int rarity;

    int uses;

    Texture2D sprite;

    bool isActive; //Para poder dibujar
    bool isEquiped;
    int wandIndentifier;

    Color color;
}Wands;

typedef struct Projectile{
    Vector2 position;
    float velocity;
    float damage;
    //Para que no se lastime a si mismo
    Player *owner;

    float size;
    bool isActive;

    Texture2D sprite;
}Projectile;

typedef struct WandSpawn{
    Vector2 position;
    int spawnRarity;
    bool hasWand;
    float spawnTimer;
    float spawnTime;

    Wands wand;

    struct WandSpawn *right;
    struct WandSpawn *left;
}WandSpawn;
void UpdateWand(Wands wands[],Player *p1,Player *p2,Vector2 offsetP1,Vector2 offsetP2,int flipPlayerOffset,Projectile projectiles[]);
void DrawWand(Wands wands[],Player *p1,Player *p2);
void UpdatePoryectile(Projectile projectiles[],Player *p1,Player *p2,Structure structures[],int structuresAmount);
void DrawProjectiles(Projectile projectiles[]);
WandSpawn *CreateSpawnNodes(Vector2 position,int rarity,float spawnTime);
WandSpawn *InsertSpawnNode(WandSpawn *root,Vector2 position,int rarity,float spawnTime);
WandSpawn *InitSpawnTree();
Wands *CreateWand(Wands worldWands[],WandSpawn *spawn);
void UpdateSpawnTree(WandSpawn *root,Player *p1,Player *p2,Wands worldWands[]);
void UpdateWandPickUp(Player *p1,Player *p2,Wands worldWands[]);