#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <stdio.h>
#include "stdlib.h"
#include "animations.h"
#include "structures.h"
#include "wands.h"

#define MAX_PROJECTILES 100
#define MAX_WORLD_WANDS 100
#define MAX_SPAWNS 8
#define SCREEN_W 1280
#define SCREEN_H 720

Animation *LoadProyectilesAnimations() {
    static Animation animations[] = {
        {
            .first=0,
            .current=0,
            .last=2,
            .speed=0.1f,
            .durationLeft=0.1f,
            .spriteSheet={0},
            .spriteWidth=128,
            .spriteHeight=72
        },
        {
            .first=0,
            .current=0,
            .last=2,
            .speed=0.1f,
            .durationLeft=0.1f,
            .spriteSheet={0},
            .spriteWidth=94,
            .spriteHeight=48
        },
        {
            .first=0,
            .current=0,
            .last=2,
            .speed=0.1f,
            .durationLeft=0.1f,
            .spriteSheet={0},
            .spriteWidth=128,
            .spriteHeight=72
        },
        {
            .first=0,
            .current=0,
            .last=2,
            .speed=0.1f,
            .durationLeft=0.1f,
            .spriteSheet={0},
            .spriteWidth=128,
            .spriteHeight=72
        }
    };

    animations[0].spriteSheet=LoadTexture("resources/Sprites/lightiningProjectile.png");
    animations[1].spriteSheet=LoadTexture("resources/Sprites/iceProjectile.png");
    animations[2].spriteSheet=LoadTexture("resources/Sprites/fireProjectile.png");
    animations[3].spriteSheet=LoadTexture("resources/Sprites/waterProjectile.png");

    return animations;
}
Sounds LoadSounds(){
    Sounds sounds={
        .fireSound=LoadSound("resources/Sounds/fireSound.wav"),
        .waterSound=LoadSound("resources/Sounds/waterSound.wav"),
        .iceSound=LoadSound("resources/Sounds/iceSound.wav"),
        .electricitySound=LoadSound("resources/Sounds/electricSound.wav")
    };

    return sounds;
}
WandTextures LoadTexturesWands(){
    WandTextures textures={
        .fireWand=LoadTexture("resources/Sprites/fireWand.png"), //Preparando la textura de las plataformas
        .iceWand=LoadTexture("resources/Sprites/iceWand.png"), //Preparando la textura del bk
        .thunderWand=LoadTexture("resources/Sprites/lightiningWand.png"), //Preparando la textura del cloud
        .waterWand=LoadTexture("resources/Sprites/waterWand.png")
    };
    return textures;
}
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
Wands *CreateWand(Wands worldWands[],WandSpawn *spawn,Animation *anims,WandTextures textures,Sounds sounds){
    int wandIdentifier;
    for(int i=1;i<MAX_WORLD_WANDS;i++){
        if(!worldWands[i].isActive){

            worldWands[i].isActive=true;
            worldWands[i].isEquiped=false;
            worldWands[i].position=spawn->position;
            worldWands[i].wandIndentifier=i;
            worldWands[i].rarity=spawn->spawnRarity;
            worldWands[i].framesPerRow=3;
            worldWands[i].wandHitboxSize=(Vector2){30,20};

            switch (worldWands[i].rarity){
           case 1: // exotico/legendario (rayo)
            worldWands[i].damage=40;
            worldWands[i].projectileSize=(Vector2){70,70};
            worldWands[i].uses=3;
            worldWands[i].velocity=3000.0f;

            worldWands[i].color=YELLOW;
            worldWands[i].size=0.7f;
            worldWands[i].proyectileAnimation=anims[0];

            worldWands[i].wandSprite=textures.thunderWand;
            worldWands[i].projectileHitboxSize=worldWands[i].projectileSize.x*0.4f;
            
            worldWands[i].wandSound=sounds.electricitySound;
            break;
            case 2: // epico (hielo)
            worldWands[i].damage=25;
            worldWands[i].projectileSize=(Vector2){70,70};
            worldWands[i].uses=6;
            worldWands[i].velocity=900.0f;

            worldWands[i].size=0.7f;
            worldWands[i].color=SKYBLUE;
            worldWands[i].proyectileAnimation=anims[1];

            worldWands[i].wandSprite=textures.iceWand;
            worldWands[i].projectileHitboxSize=worldWands[i].projectileSize.x*0.4f;

            worldWands[i].wandSound=sounds.iceSound;
            break;
            case 3: // raro (fuego)
            worldWands[i].damage=18;
            worldWands[i].projectileSize=(Vector2){70,70};
            worldWands[i].uses=8;
            worldWands[i].velocity=650.0f;

            worldWands[i].size=0.6f;
            worldWands[i].color=ORANGE;
            worldWands[i].proyectileAnimation=anims[2];

            worldWands[i].wandSprite=textures.fireWand;
            worldWands[i].projectileHitboxSize=worldWands[i].projectileSize.x*0.4f;

            worldWands[i].wandSound=sounds.fireSound;
            break;
            case 4: // comun (agua)
            worldWands[i].damage=10;
            worldWands[i].projectileSize=(Vector2){100,100};
            worldWands[i].uses=12;
            worldWands[i].velocity=500.0f;

            worldWands[i].size=0.6f;
            worldWands[i].color=BLUE;
            worldWands[i].proyectileAnimation=anims[3];

            worldWands[i].wandSprite=textures.waterWand;
            worldWands[i].projectileHitboxSize=worldWands[i].projectileSize.x*0.6f;

            worldWands[i].wandSound=sounds.waterSound;
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
void UpdateSpawnTree(WandSpawn *root,Player *p1,Player *p2,Wands worldWands[],Animation *proAnims,WandTextures textures,Sounds sounds){
    if(root==NULL) return;

    float dt=GetFrameTime();

    Wands *newWand=NULL;
    
    if(!root->hasWand){
        root->spawnTimer-=dt;
        if(root->spawnTimer<=0.0f){
            newWand=CreateWand(worldWands,root,proAnims,textures,sounds);
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

    UpdateSpawnTree(root->left,p1,p2,worldWands,proAnims,textures,sounds);
    UpdateSpawnTree(root->right,p1,p2,worldWands,proAnims,textures,sounds);
    return;
}
void UpdateWandPickUp(Player *p1,Player *p2,Wands worldWands[]){
    Rectangle p1Rect={p1->position.x,p1->position.y,p1->size.x,p1->size.y};
    Rectangle p2Rect={p2->position.x,p2->position.y,p2->size.x,p2->size.y};

    for(int i=0;i<MAX_WORLD_WANDS;i++){
            Rectangle wandRect={worldWands[i].position.x,worldWands[i].position.y,worldWands[i].wandHitboxSize.x,worldWands[i].wandHitboxSize.y};

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
void UpdateProjectile(Projectile projectiles[],Player *p1,Player *p2,Structure structures[],int structuresAmount,Sound damageSound){
    float dt=GetFrameTime();
    
    for(int i=0;i<MAX_PROJECTILES;i++){
        if(projectiles[i].isActive){
            AnimationUpdate(&projectiles[i].animation);

            projectiles[i].position.x+=projectiles[i].velocity*dt;

            Player *tarjet=(projectiles[i].owner==p1) ?p2 :p1;
            Rectangle pRect={tarjet->position.x,tarjet->position.y,tarjet->size.x,tarjet->size.y};

            Vector2 projectileCenter={projectiles[i].position.x + projectiles[i].animation.spriteWidth / 2.0f,projectiles[i].position.y + projectiles[i].animation.spriteHeight / 2.0f};
            float projectileRadius = projectiles[i].hitboxSize / 2.0f; 
            
            //Colisiones con jugador
            if(CheckCollisionCircleRec(projectileCenter,projectileRadius,pRect)){
                tarjet->helth-=projectiles[i].damage;
                //Sonido de dano
                PlaySound(damageSound);
                tarjet->damageTimer=tarjet->damageTime; //Inicio el efecto de dano

                //Efecto de empuje de la varita de agua
                if(projectiles[i].rarity==4){
                    tarjet->firstDashPos=tarjet->position.x;
                    tarjet->velocity.x+=(projectiles[i].velocity>0) ?1000 :-1000;
                    tarjet->lastDashTime=GetTime();
                    tarjet->isDashing=true;
                }
                if(projectiles[i].rarity==3){
                    KeyboardKey aux=tarjet->keyLeft;
                    tarjet->keyLeft=tarjet->keyRight;
                    tarjet->keyRight=aux;
                    tarjet->invertControlsTimer=tarjet->invertControlsTimerTime;

                }
                if(projectiles[i].rarity==2){
                    tarjet->isFreezedTimer=tarjet->isFreezedTimerTime;
                }
                projectiles[i].isActive=false;
            }
            
            //Colisiones con estructuras
            for(int j=0;j<structuresAmount;j++){
                if(CheckCollisionCircleRec(projectileCenter,projectileRadius,structures[j].rect)){
                    if(projectiles[i].rarity!=1)projectiles[i].isActive=false; //El de rayo atraviesa estructuras
                }
            }

            if(projectiles[i].position.x>SCREEN_W+projectiles[i].size.x || projectiles[i].position.x<-projectiles[i].size.x) projectiles[i].isActive=false;
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
                float tipOffset = (wands[i].wandSprite.height*wands[i].size) / 2.0f; 
                float cos45     = 0.7071f;

                // Punta de la varita en world space
                float tipX = wands[i].position.x + (tipOffset * cos45 * p1->direction);
                float tipY = wands[i].position.y - (tipOffset * cos45);

                // Centramos el proyectil sobre la punta
                projectiles[j].position.x = tipX - wands[i].projectileSize.x / 2.0f;
                projectiles[j].position.y = (tipY - wands[i].projectileSize.y / 2.0f)+25;


                projectiles[j].velocity=wands[i].velocity;
                projectiles[j].velocity*=p1->direction;
                projectiles[j].size=wands[i].projectileSize;
                projectiles[j].hitboxSize=wands[i].projectileHitboxSize;
                projectiles[j].damage=wands[i].damage;
                projectiles[j].owner=p1; //El que lanzo el hechizo

                projectiles[j].animation=wands[i].proyectileAnimation;
                projectiles[j].framesPerRow=wands[i].framesPerRow;
                projectiles[j].rarity=wands[i].rarity;

                projectiles[j].isActive=true;
                wands[i].uses--;

                //Reproduzco el sonido
                PlaySound(wands[i].wandSound);
                j=MAX_PROJECTILES;
                }
            }
        }
        if(IsKeyPressed(p2->keyAttack) && wands[i].uses>0 && wands[i].isEquiped && wands[i].wandIndentifier==p2->wandIdentifier[0]){
        for(int j=0;j<MAX_PROJECTILES;j++){
            if(!projectiles[j].isActive){

                float tipOffset = (wands[i].wandSprite.height*wands[i].size) / 2.0f;   // mitad del largo de la varita
                float cos45     = 0.7071f;

                // Punta de la varita en world space
                float tipX = wands[i].position.x + (tipOffset * cos45 * p1->direction);
                float tipY = wands[i].position.y - (tipOffset * cos45)+25;

                // Centramos el proyectil sobre la punta
                projectiles[j].position.x = tipX - wands[i].projectileSize.x / 2.0f;
                projectiles[j].position.y = tipY - wands[i].projectileSize.y / 2.0f;


                projectiles[j].velocity=wands[i].velocity;
                projectiles[j].velocity*=p2->direction;
                projectiles[j].size=wands[i].projectileSize;
                projectiles[j].hitboxSize=wands[i].projectileHitboxSize;
                projectiles[j].damage=wands[i].damage;
                projectiles[j].owner=p2; //El que lanzo el hechizo

                projectiles[j].animation=wands[i].proyectileAnimation;
                projectiles[j].framesPerRow=wands[i].framesPerRow;
                projectiles[j].rarity=wands[i].rarity;

                projectiles[j].isActive=true;
                wands[i].uses--;

                //Reproduzco el sonido
                PlaySound(wands[i].wandSound);
                j=MAX_PROJECTILES;
                }
            }
        }
    }
}

//=========================DIBUJADO
void DrawWand(Wands wands[], Player *p1, Player *p2, WandTextures textures)
{
    for (int i = 0; i < MAX_WORLD_WANDS; i++) {
        if (!wands[i].isActive) continue;

        Player *owner = NULL;
        if (wands[i].wandIndentifier == p1->wandIdentifier[0]) owner = p1;
        else if (wands[i].wandIndentifier == p2->wandIdentifier[0]) owner = p2;

        float dw=(float)wands[i].wandSprite.width;
        float dh=(float)wands[i].wandSprite.height;
        float scale=wands[i].size;
        float sdw=dw*scale;
        float sdh=dh*scale;
        Vector2 origin={sdw/2.0f,sdh/2.0f};
        float wandFloatEffect = sinf(GetTime() * 3.0f + i * 1.2f) * 4.0f;
        float spawnPosOffset=-30.0f;

        if (wands[i].isEquiped && owner != NULL) {
            Rectangle src = (owner->direction < 0) ? (Rectangle){ dw, 0, -dw, dh } : (Rectangle){ 0,  0,  dw, dh };
            Rectangle dst = { wands[i].position.x, wands[i].position.y, sdw, sdh };
            float rotation = 45.0f * owner->direction;
            DrawTexturePro(wands[i].wandSprite, src, dst, origin, rotation, WHITE);

        } else if (!wands[i].isEquiped) {
            Rectangle src = { 0, 0, dw, dh };
            Rectangle dst = {wands[i].position.x,wands[i].position.y+wandFloatEffect+spawnPosOffset, sdw, sdh };
            DrawTexturePro(wands[i].wandSprite, src, dst, (Vector2){0,0}, 45.0f, WHITE);
        }
    }
}

void DrawProjectiles(Projectile projectiles[],Player *p1,Player *p2){
    for(int i=0;i<MAX_PROJECTILES;i++){
       if(projectiles[i].isActive){
            int direction=(projectiles[i].velocity>0) ?1 :-1;
            direction*=(projectiles[i].owner==p1 && projectiles[i].rarity!=4) ?-1 :1;
            // if() direction*=-1; //El de agua
            
            DrawAnimation(&projectiles[i].animation,projectiles[i].framesPerRow,projectiles[i].size,projectiles[i].position,projectiles[i].animation.spriteWidth,projectiles[i].animation.spriteHeight,direction,WHITE);
            
        }
    }
}