#include "raylib.h"
#include "player.h"
#include "structures.h"

#define MAX_PROJECTILES 100
#define MAX_WORLD_WANDS 100
#define MAX_SPAWNS 8
#define SCREEN_W 1280
#define SCREEN_H 720

typedef struct Wands{
    float damage;
    float velocity;
    Vector2 spawnPos;
    Vector2 position;
    float size;
    Vector2 wandHitboxSize;
    Vector2 projectileSize;
    float projectileHitboxSize;
    int rarity;

    int uses;

    bool isActive; //Para poder dibujar
    bool isEquiped;
    int wandIndentifier;

    Color color;

    Texture2D proyectileSprite;
    Animation proyectileAnimation;
    int framesPerRow;

    Texture2D wandSprite;
    
    Sound wandSound;
}Wands;

typedef struct Projectile{
    Vector2 position;
    float velocity;
    float damage;

    //Para que no se lastime a si mismo
    Player *owner;

    Vector2 size;
    float hitboxSize;
    bool isActive;

    Animation animation;
    int framesPerRow;
    int rarity;
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

typedef struct WandTextures{
    Texture2D fireWand;
    Texture2D thunderWand;
    Texture2D iceWand;
    Texture2D waterWand;
}WandTextures;
typedef struct Sounds{
    Sound waterSound;
    Sound fireSound;
    Sound iceSound;
    Sound electricitySound;
}Sounds;

typedef struct ProyectilesTextures{
    Texture2D fireProyectile;
    Texture2D thunderProyectile;
    Texture2D iceProyectile;
    Texture2D waterPryectile;
}ProyectilesTextures;
Animation *LoadProyectilesAnimations();
void UpdateWand(Wands wands[],Player *p1,Player *p2,Vector2 offsetP1,Vector2 offsetP2,int flipPlayerOffset,Projectile projectiles[]);
void DrawWand(Wands wands[],Player *p1,Player *p2,WandTextures textures);
void UpdateProjectile(Projectile projectiles[],Player *p1,Player *p2,Structure structures[],int structuresAmount,Sound damageSound);
void DrawProjectiles(Projectile projectiles[],Player *p1,Player *p2);
WandSpawn *CreateSpawnNodes(Vector2 position,int rarity,float spawnTime);
WandSpawn *InsertSpawnNode(WandSpawn *root,Vector2 position,int rarity,float spawnTime);
WandSpawn *InitSpawnTree();
WandTextures LoadTexturesWands();
Wands *CreateWand(Wands worldWands[],WandSpawn *spawn,Animation *anims,WandTextures textures,Sounds sounds);
void UpdateSpawnTree(WandSpawn *root,Player *p1,Player *p2,Wands worldWands[],Animation *proAnims,WandTextures textures,Sounds sounds);
void UpdateWandPickUp(Player *p1,Player *p2,Wands worldWands[]);
Sounds LoadSounds();