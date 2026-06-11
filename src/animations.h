#include "raylib.h"

typedef struct Animation{
    int first;
    int current;
    int last;

    float speed;
    float durationLeft;

    Texture2D spriteSheet;

    int spriteWidth;
}Animation;

void AnimationUpdate(Animation *self);
Rectangle AnimationFrame(Animation *self,int numFramesPerRow,int spriteWidth);
void DrawAnimation(Animation *self,int numFramesPerRow,Vector2 pSize,Vector2 pPos,int spriteWidth,int pDirection);