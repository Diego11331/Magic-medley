#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "animations.h"

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
Rectangle AnimationFrame(Animation *self,int numFramesPerRow,int spriteWidth,int spriteHeight){
    int x=(self->current%numFramesPerRow)*spriteWidth;
    int y=(self->current/numFramesPerRow)*spriteHeight;

    return (Rectangle){.x=x,.y=y,.width=spriteWidth,.height=spriteHeight};
}
void DrawAnimation(Animation *self,int numFramesPerRow,Vector2 pSize,Vector2 pPos,int spriteWidth,int spriteHeight,int pDirection,Color color){
    Rectangle playerFrame=(Rectangle)AnimationFrame(self,numFramesPerRow,spriteWidth,spriteHeight);
    playerFrame.width*=pDirection;
    DrawTexturePro(self->spriteSheet,playerFrame,(Rectangle){pPos.x,pPos.y,pSize.x,pSize.y},(Vector2){0,0},0,color);
}