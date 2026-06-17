#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <stdio.h>
#include "stdlib.h"
#include "animations.h"
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

WandSpawn *CreateSpawnNodes(Vector2 position,int rarity,float spawnTime){
    WandSpawn *node=(WandSpawn *)malloc(sizeof(WandSpawn));

    if(node==NULL) return NULL;

    node->position=position;
    node->spawnRarity=rarity;
    node->hasWand=false;
    node->spawnTimer=spawnTime;
    node->spawnTime=spawnTime;

    node->right=NULL;
    node->left=NULL;
    return node;
}
WandSpawn *InsertSpawnNode(WandSpawn *root,Vector2 position,int rarity,float spawnTime){
    if(root==NULL) return root=CreateSpawnNodes(position,rarity,spawnTime);

    if(rarity<root->spawnRarity){
        root->left=InsertSpawnNode(root->left,position,rarity,spawnTime);
    }else{
        root->right=InsertSpawnNode(root->right,position,rarity,spawnTime);
    }
    return root;
}
WandSpawn *InitSpawnTree(){
    WandSpawn *root=NULL;

    // Comun
    root = InsertSpawnNode(root, (Vector2){112,  528}, 4,  3.0f);
    root = InsertSpawnNode(root, (Vector2){1040, 528}, 4,  3.0f);
    root = InsertSpawnNode(root, (Vector2){496,  488}, 4,  3.0f);
    root = InsertSpawnNode(root, (Vector2){688,  488}, 4,  3.0f);

    // Poco comun
    root = InsertSpawnNode(root, (Vector2){240,  398}, 3,  6.0f);
    root = InsertSpawnNode(root, (Vector2){880,  398}, 3,  6.0f);
    root = InsertSpawnNode(root, (Vector2){576,  358}, 3,  6.0f);

    // Epico
    root = InsertSpawnNode(root, (Vector2){272,  138}, 2, 10.0f);
    root = InsertSpawnNode(root, (Vector2){912,  138}, 2, 10.0f);

    // Legendario
    root = InsertSpawnNode(root, (Vector2){624,   16}, 1, 20.0f);

    return root;
}
Wands *CreateWand(Wands worldWands[],WandSpawn *spawn){
    int wandIdentifier;
    for(int i=1;i<=MAX_WORLD_WANDS;i++){
        if(!worldWands[i].isActive){
            worldWands[i].isActive=true;
            worldWands[i].isEquiped=false;
            worldWands[i].position=spawn->position;
            worldWands[i].wandIndentifier=i;
            worldWands[i].rarity=spawn->spawnRarity;
            switch (worldWands[i].rarity){
            case 1: //exotico (rayo)
                worldWands[i].damage=25;
                worldWands[i].projectileSize=20;
                worldWands[i].size=(Vector2){5,25};
                worldWands[i].uses=5;
                worldWands[i].velocity=3000.0f;
                worldWands[i].color=YELLOW;
                break;
            case 2: //epico
                worldWands[i].damage=20;
                worldWands[i].projectileSize=10;
                worldWands[i].size=(Vector2){5,25};
                worldWands[i].uses=10;
                worldWands[i].velocity=500;
                worldWands[i].color=RED;
                break;
            case 3: //raro
                worldWands[i].damage=15;
                worldWands[i].projectileSize=10;
                worldWands[i].size=(Vector2){5,25};
                worldWands[i].uses=10;
                worldWands[i].velocity=500;
                worldWands[i].color=BLUE;
                break;
            case 4: //comun
                worldWands[i].damage=10;
                worldWands[i].projectileSize=10;
                worldWands[i].size=(Vector2){5,25};
                worldWands[i].uses=10;
                worldWands[i].velocity=500;
                worldWands[i].color=WHITE;
                break;
            default:
                break;
            }
            return &worldWands[i];
        }
    }
    return NULL;
}
//=============================ACTUALIZACION
void UpdateSpawnTree(WandSpawn *root,Player *p1,Player *p2,Wands worldWands[]){
    if(root==NULL) return;

    float dt=GetFrameTime();

    Wands *newWand=NULL;
    
    if(!root->hasWand){
        root->spawnTimer-=dt;
        if(root->spawnTimer<=0.0f){
            newWand=CreateWand(worldWands,root);
            if(newWand!=NULL)root->hasWand=true;
        }
    }else{
        bool wandStillExist=false;
        for(int i=0;i<MAX_WORLD_WANDS;i++){
            if(worldWands[i].position.x==root->position.x && worldWands[i].position.y==root->position.y && worldWands[i].isActive){
                wandStillExist=true;
                i=MAX_WORLD_WANDS;
            }
        }
        if(!wandStillExist){
            root->spawnTimer=root->spawnTime;
            root->hasWand=false;
        }
    }

    UpdateSpawnTree(root->left,p1,p2,worldWands);
    UpdateSpawnTree(root->right,p1,p2,worldWands);
    return;
}
void UpdateWandPickUp(Player *p1,Player *p2,Wands worldWands[]){
    Rectangle p1Rect={p1->position.x,p1->position.y,p1->size.x,p1->size.y};
    Rectangle p2Rect={p2->position.x,p2->position.y,p2->size.x,p2->size.y};

    for(int i=0;i<MAX_WORLD_WANDS;i++){
            Rectangle wandRect={worldWands[i].position.x,worldWands[i].position.y,worldWands[i].size.x,worldWands[i].size.y};

            if(worldWands[i].isActive && !worldWands[i].isEquiped && IsKeyPressed(p1->keyPick) && CheckCollisionRecs(wandRect,p1Rect)){
                if(p1->wandIdentifier[0]!=0) worldWands[p1->wandIdentifier[0]].isEquiped=false;
                p1->wandIdentifier[0]=worldWands[i].wandIndentifier;
                worldWands[i].isEquiped=true;
                p1->hasWand=true;
            }

            if(worldWands[i].isActive && !worldWands[i].isEquiped && IsKeyPressed(p2->keyPick) && CheckCollisionRecs(wandRect,p2Rect)){
                if(p2->wandIdentifier[0]!=0) worldWands[p2->wandIdentifier[0]].isEquiped=false;
                p2->wandIdentifier[0]=worldWands[i].wandIndentifier;
                worldWands[i].isEquiped=true;
                p2->hasWand=true;
            }
        }
}
void UpdatePoryectile(Projectile projectiles[],Player *p1,Player *p2,Structure structures[],int structuresAmount){
    float dt=GetFrameTime();
    
    for(int i=0;i<MAX_PROJECTILES;i++){
        if(projectiles[i].isActive){
            projectiles[i].position.x+=projectiles[i].velocity*dt;

            Player *tarjet=(projectiles[i].owner==p1) ?p2 :p1;
            Rectangle pRect={tarjet->position.x,tarjet->position.y,tarjet->size.x,tarjet->size.y};

            //Colisiones con jugador
            if(CheckCollisionCircleRec(projectiles[i].position,projectiles[i].size,pRect)){
                tarjet->helth-=projectiles[i].damage;
                tarjet->damageTimer=tarjet->damageTime; //Inicio el efecto de dano
                projectiles[i].isActive=false;
            }
            //Colisiones con estructuras
            for(int j=0;j<structuresAmount;j++){
                if(CheckCollisionCircleRec(projectiles[i].position,projectiles[i].size,structures[j].rect)){
                    projectiles[i].isActive=false;
                }
            }
        }
        
    }
}
void UpdateWand(Wands wands[],Player *p1,Player *p2,Vector2 offsetP1,Vector2 offsetP2,int flipPlayerOffset,Projectile projectiles[]){
    if(p1->direction<0) offsetP1.x+=flipPlayerOffset;
    if(p2->direction<0) offsetP2.x+=flipPlayerOffset;
    
    for(int i=0;i<MAX_WORLD_WANDS;i++){
        if(wands[i].isActive && wands[i].isEquiped && wands[i].wandIndentifier==p1->wandIdentifier[0]){
            wands[i].position.x=(p1->position.x+p1->size.x/2)+(offsetP1.x*p1->direction);
            wands[i].position.y=p1->position.y-offsetP1.y;
        }
        if(wands[i].isActive && wands[i].isEquiped && wands[i].wandIndentifier==p2->wandIdentifier[0]){
            wands[i].position.x=(p2->position.x+p2->size.x/2)+(offsetP2.x*p2->direction);
            wands[i].position.y=p2->position.y-offsetP2.y;
        }

        if(IsKeyPressed(p1->keyAttack) && wands[i].uses>0 && wands[i].isEquiped && wands[i].wandIndentifier==p1->wandIdentifier[0]){
        for(int j=0;j<MAX_PROJECTILES;j++){
            if(!projectiles[j].isActive){

                projectiles[j].position.x=p1->position.x+10;
                projectiles[j].position.y=p1->position.y+15;
                projectiles[j].velocity=wands[i].velocity;
                projectiles[j].velocity*=p1->direction;
                projectiles[j].size=wands[i].projectileSize;
                projectiles[j].damage=wands[i].damage;
                projectiles[j].owner=p1; //El que lanzo el hechizo
                projectiles[j].isActive=true;
                wands[i].uses--;
                j=MAX_PROJECTILES;
                }
            }
        }
        if(IsKeyPressed(p2->keyAttack) && wands[i].uses>0 && wands[i].isEquiped && wands[i].wandIndentifier==p2->wandIdentifier[0]){
        for(int j=0;j<MAX_PROJECTILES;j++){
            if(!projectiles[j].isActive){

                projectiles[j].position.x=p2->position.x+10;
                projectiles[j].position.y=p2->position.y+15;
                projectiles[j].velocity=wands[i].velocity;
                projectiles[j].velocity*=p2->direction;
                projectiles[j].size=wands[i].projectileSize;
                projectiles[j].owner=p2; //El que lanzo el hechizo
                projectiles[j].isActive=true;
                wands[i].uses--;
                j=MAX_PROJECTILES;
                }
            }
        }
    }
}



//=========================DIBUJADO
void DrawWand(Wands wands[],Player *p1,Player *p2){
    for(int i=0;i<MAX_WORLD_WANDS;i++){
        Player *owner=NULL;
        float wandFloatEffect=sinf(GetTime()*3.0f+i*1.2f)*4.0f;
        if(wands[i].wandIndentifier==p1->wandIdentifier[0]) owner=p1;
        else if(wands[i].wandIndentifier==p2->wandIdentifier[0]) owner=p2;

        if(wands[i].isActive && wands[i].isEquiped){
            Rectangle rect=(Rectangle){wands[i].position.x,wands[i].position.y,wands[i].size.x,wands[i].size.y};
            float rotation=45*owner->direction;
            DrawRectanglePro(rect,(Vector2){0,0},rotation,wands[i].color);
        }else if(wands[i].isActive && !wands[i].isEquiped){
            Rectangle rect=(Rectangle){wands[i].position.x,wands[i].position.y+wandFloatEffect,wands[i].size.x,wands[i].size.y};
            float rotation=45;
            DrawRectanglePro(rect,(Vector2){0,0},rotation,wands[i].color);
        }
    }
}

void DrawProjectiles(Projectile projectiles[]){
    for(int i=0;i<MAX_PROJECTILES;i++){
       if(projectiles[i].isActive){
            DrawCircle(projectiles[i].position.x,projectiles[i].position.y,projectiles[i].size,BLUE);
        }
    }

}