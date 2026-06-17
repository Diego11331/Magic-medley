#include "player.h"
#include "wands.h"
#include "structures.h"

#define SCREEN_W 1280
#define SCREEN_H 720

void ResetLevel(Player *p1,Player *p2,int screenHeight,Vector2 p1SpawnPos,Vector2 p2SpawnPos,float initialHelth,Wands wands[]){
    if((p1->helth<=0 || p2->helth<=0) || (p1->position.y>screenHeight+300 || p2->position.y>screenHeight+300)){
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

        for(int i=0;i<MAX_WORLD_WANDS;i++){
            wands[i]=(Wands){0};
        }
    }

}
void DrawPlayerInfo(Player *p1,Player *p2,Wands wands[]){
    int padding=0;
    for(int i=0;i<MAX_WORLD_WANDS;i++){
        
        if(wands[i].isActive && wands[i].isEquiped){
            if(wands[i].wandIndentifier==p1->wandIdentifier[0] || wands[i].wandIndentifier==p1->wandIdentifier[1]){
                DrawText(TextFormat("Uses: %i",wands[i].uses),10,50,50,RED);
                for(int j=0;j<wands[i].uses;j++){
                    // DrawRectangle(padding,2,5,50,RED);
                    padding+=10;
                }
                
            }else if(wands[i].wandIndentifier==p2->wandIdentifier[0] || wands[i].wandIndentifier==p2->wandIdentifier[1]){
                DrawText(TextFormat("Uses: %i",wands[i].uses),1000,50,50,BLUE);
            }
        }
    }
    int helthBarMultiplier=5;
    DrawRectangle(0,10,p1->helth*helthBarMultiplier,50,RED);
    DrawRectangle(SCREEN_W-p2->helth,10,p2->helth*helthBarMultiplier,50,BLUE);
    // DrawText(TextFormat("Life: %i",p1->helth),10,10,50,RED);
    // DrawText(TextFormat("Life: %i",p2->helth),1000,10,50,BLUE);
}