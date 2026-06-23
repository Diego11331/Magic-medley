#include "player.h"
#include "stdio.h"
#include "wands.h"
#include "structures.h"
#include "mainMenu.h"

#define SCREEN_W 1280
#define SCREEN_H 720

bool DrawMenuButton(MenuButton *btn, bool isPrimary,Color titleColor,Color subtitleColor,Texture2D titleTex){
    //Dibujando los titulos
    const char *title = "MAGIC MADLEY";
    int titleFontSize = 64;
    // DrawText(title, (SCREEN_W - titleWidth) / 2, 170, titleFontSize, titleColor);
    
    DrawTexture(titleTex,(SCREEN_W - titleTex.width) / 2,20,WHITE);

    const char *subtitle = "a magic adventure";
    int subFontSize = 20;
    
    // DrawText(subtitle, (SCREEN_W - subWidth) / 2, 245, subFontSize, subtitleColor);
    //Logica de los botones
    Vector2 mouse = GetMousePosition();
    btn->isHovered = CheckCollisionPointRec(mouse, btn->bounds);
    btn->isPressed = btn->isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool clicked = btn->isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
 
    Color fill, border, text;
 
    if (isPrimary) {
        fill = btn->isPressed ? (Color){ 70, 45, 130, 255 }:btn->isHovered ? (Color){ 110, 80, 190, 255 }:(Color){ 90, 60, 160, 255 };
        border=(Color){ 180, 150, 255, 255 };
        text=WHITE;
    } else {
        fill = btn->isPressed ? (Color){ 45, 40, 70, 255 }:btn->isHovered ? (Color){ 65, 58, 100, 255 }:(Color){ 50, 45, 80, 255 };
        border = (Color){ 90, 80, 130, 255 };
        text   = (Color){ 225, 220, 240, 255 };
    }
 
    DrawRectangleRounded(btn->bounds, 0.3f, 8, fill);
    DrawRectangleLinesEx(btn->bounds, 2.0f, border);
 
    int fontSize  = 22;
    int textWidth = MeasureText(btn->label, fontSize);
    Vector2 textPos = {
        btn->bounds.x + (btn->bounds.width  - textWidth) / 2.0f,
        btn->bounds.y + (btn->bounds.height - fontSize)  / 2.0f
    };
    DrawText(btn->label, (int)textPos.x, (int)textPos.y, fontSize, text);
 
    return clicked;
}