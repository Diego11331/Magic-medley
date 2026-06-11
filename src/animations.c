#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "animations.h"

typedef struct Animation{
    int first;
    int current;
    int last;

    float speed;
    float durationLeft;

    Texture2D spriteSheet;

    int spriteWidth;
}Animation;

void AnimationUpdate(Animation *self){
    float dt=GetFrameTime();
    self->durationLeft-=dt;
    if(self->durationLeft<=0){
        self->current++;
        self->durationLeft=self->speed;
    }
    if(self->current>self->last){
        self->current=self->first;
    }
}
Rectangle AnimationFrame(Animation *self,int numFramesPerRow,int spriteWidth){
    int x=(self->current%numFramesPerRow)*spriteWidth;
    int y=(self->current/numFramesPerRow)*spriteWidth;

    return (Rectangle){.x=x,.y=y,.width=spriteWidth,.height=spriteWidth};
}
void DrawAnimation(Animation *self,int numFramesPerRow,Vector2 pSize,Vector2 pPos,int spriteWidth,int pDirection){
    Rectangle playerFrame=(Rectangle)AnimationFrame(self,numFramesPerRow,spriteWidth);
    playerFrame.width*=pDirection;
    DrawTexturePro(self->spriteSheet,playerFrame,(Rectangle){pPos.x,pPos.y,pSize.x,pSize.y},(Vector2){0,0},0,WHITE);
}