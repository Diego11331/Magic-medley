#include "player.h"
#include "stdio.h"
#include <stdlib.h>
#include "wands.h"
#include "structures.h"
#include "gameManager.h"

#define SCREEN_W 1280
#define SCREEN_H 720

bool WriteFileScore(int p1Score,int p2Score){
	FILE *file = fopen("scores.csv","a");
	
	if(file==NULL) return false;
	
	fprintf(file,"%i,%i\n",p1Score,p2Score);
	
	fclose(file);
	return true;
}
void ResetTimerSapwns(WandSpawn *root){
    if(root==NULL) return;
    root->spawnTimer=root->spawnTime;

    ResetTimerSapwns(root->left);
    ResetTimerSapwns(root->right);
}
void ResetLevel(Player *p1,Player *p2,int screenHeight,Vector2 p1SpawnPos,Vector2 p2SpawnPos,float initialHelth,Wands wands[],WandSpawn *root,GameState currentState,int *p1Score,int *p2Score,float *resetTimer,float *resetTimerTime){
    bool p1Die=(p1->helth<=0);
    bool p2Die=(p2->helth<=0);
    bool playerGoToMenu=currentState==GAME && IsKeyDown(KEY_BACKSPACE);

    if(p1->position.y>screenHeight+300){
        p1Die=true;
        p1->helth=0;
    } 
    if(p2->position.y>screenHeight+300){
        p1->helth=0;
        p2Die=true;
    } 

    if(p1Die) (*p2Score)++;
    if(p2Die) (*p1Score)++;

    if(p1Die || p2Die || playerGoToMenu){
        //Congelo los jugadores
        p1->velocity.x=0;
        p2->velocity.x=0;

        p1->velocity.y=0;
        p2->velocity.y=0;

        *resetTimer-=GetFrameTime();

        if(*resetTimer<=0){
            p1->position=p1SpawnPos;
            p2->position=p2SpawnPos;
            p1->helth=initialHelth;
            p2->helth=initialHelth;
            p1->hasWand=false;
            p1->wandIdentifier[0]=0;
            p1->wandIdentifier[1]=0;
            p2->hasWand=false;
            p2->wandIdentifier[0]=0;
            p2->wandIdentifier[1]=0;

            //Para que se desactiven los efectos
            p1->isFreezedTimer=0;
            p2->isFreezedTimer=0;

            p1->invertControlsTimer=0;
            p2->invertControlsTimer=0;

            for(int i=0;i<MAX_WORLD_WANDS;i++) wands[i]=(Wands){0};
            
            ResetTimerSapwns(root);

            if(playerGoToMenu){
                WriteFileScore(*p1Score,*p2Score);
                *p1Score=0;
                *p2Score=0;
            }
            *resetTimer=*resetTimerTime;
        }
    }
}

void DrawPlayerInfo(Player *p1,Player *p2,Wands wands[],Texture2D heartTex,Texture2D swordTex){
    int paddingP1=swordTex.width;
    int paddingP2=SCREEN_W-swordTex.width;
    int usesBarWidth=5;
    int usesBarHeight=64;

    DrawTextureEx(swordTex,(Vector2){0,70},0,1,GRAY);
    DrawTextureEx(swordTex,(Vector2){SCREEN_W-swordTex.width,70},0,1,GRAY);

    for(int i=0;i<MAX_WORLD_WANDS;i++){
        
        if(wands[i].isActive && wands[i].isEquiped){
            if(wands[i].wandIndentifier==p1->wandIdentifier[0] || wands[i].wandIndentifier==p1->wandIdentifier[1]){
                    for(int j=0;j<wands[i].uses;j++){
                    paddingP1+=10;
                    DrawRectangle(paddingP1,70,usesBarWidth,usesBarHeight,GRAY);
                }
            }else if(wands[i].wandIndentifier==p2->wandIdentifier[0] || wands[i].wandIndentifier==p2->wandIdentifier[1]){
                for(int j=0;j<wands[i].uses;j++){
                    paddingP2-=10;
                    DrawRectangle(paddingP2,70,usesBarWidth,usesBarHeight,GRAY);
                }
            }
        }
    }
    int helthBarMultiplier=4;
    int helthBarP2=helthBarMultiplier*p2->helth;

    DrawTextureEx(heartTex,(Vector2){0,0},0,1,RED);
    DrawRectangle(heartTex.width,5,p1->helth*helthBarMultiplier,50,RED);

    DrawTextureEx(heartTex,(Vector2){SCREEN_W-heartTex.width,0},0,1,RED);
    DrawRectangle(SCREEN_W-helthBarP2-heartTex.width,5,helthBarP2,50,RED);

    const char *winerText = (p1->helth<=0) ?"Player 2 won" :"Player 1 won";
    int fontSize=150;
    int titleWidth = MeasureText(winerText, fontSize);

    if(p1->helth<=0){
        DrawText(winerText,(SCREEN_W-titleWidth)/2,245,fontSize,BLACK);
    }else if(p2->helth<=0){
        DrawText(winerText,(SCREEN_W-titleWidth)/2,245,fontSize,BLACK);
    }
    
}