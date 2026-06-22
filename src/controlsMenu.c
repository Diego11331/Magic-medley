#include "raylib.h"
#include "player.h"   /* ajusta esto al header donde tengas definido el struct Player */
#include <stdbool.h>
#include <stdio.h>
#include "gameManager.h"

/* Convierte una tecla de raylib en un texto legible para mostrarla en pantalla */
static const char *KeyToString(int key)
{
    static char letterBuf[2];
    if (key >= KEY_A && key <= KEY_Z) {
        letterBuf[0] = (char)key;
        letterBuf[1] = '\0';
        return letterBuf;
    }
    switch (key) {
        case KEY_LEFT:        return "Left arrow";
        case KEY_RIGHT:       return "Right arrow";
        case KEY_UP:          return "Up arrow";
        case KEY_DOWN:        return "Down arrow";
        case KEY_SPACE:       return "Space";
        case KEY_LEFT_SHIFT:  return "Left shift";
        case KEY_RIGHT_SHIFT: return "Right shift";
        default:              return "?";
    }
}

/* Dibuja una fila "accion ........ tecla" dentro de una tarjeta de jugador */
static void DrawControlRow(float x, float y, float width, const char *action, int key, Color textColor)
{
    int fontSize = 20;
    DrawText(action, (int)x, (int)y, fontSize, textColor);
    const char *keyLabel = KeyToString(key);
    int keyWidth = MeasureText(keyLabel, fontSize);
    DrawText(keyLabel, (int)(x + width - keyWidth), (int)y, fontSize, textColor);
}

/* Dibuja la tarjeta completa con los controles de un jugador */
static void DrawPlayerCard(Rectangle card, Player *player, const char *playerLabel)
{
    DrawRectangleRounded(card, 0.08f, 8, (Color){ 38, 32, 60, 255 });
    DrawRectangleLinesEx(card, 2.0f, player->color);

    float pad = 24.0f;
    float innerWidth = card.width - pad * 2;

    DrawCircle((int)(card.x + pad + 8), (int)(card.y + pad + 8), 8, player->color);
    DrawText(playerLabel, (int)(card.x + pad + 26), (int)(card.y + pad), 22, WHITE);

    float rowY = card.y + pad + 44;
    float rowGap = 34;
    Color rowColor = (Color){ 220, 215, 235, 255 };

    DrawControlRow(card.x + pad, rowY + rowGap * 0, innerWidth, "Move left",  player->keyLeft,   rowColor);
    DrawControlRow(card.x + pad, rowY + rowGap * 1, innerWidth, "Move right", player->keyRight,  rowColor);
    DrawControlRow(card.x + pad, rowY + rowGap * 2, innerWidth, "Jump",       player->keyJump,   rowColor);
    DrawControlRow(card.x + pad, rowY + rowGap * 3, innerWidth, "Pick up",    player->keyPick,   rowColor);
    DrawControlRow(card.x + pad, rowY + rowGap * 4, innerWidth, "Attack",     player->keyAttack, rowColor);

    Color noteColor = (Color){ 150, 135, 190, 255 };
    int noteFontSize = 16;

    char jumpNote[64];
    snprintf(jumpNote, sizeof(jumpNote), "Double jump: press %s twice in the air",KeyToString(player->keyJump));
    DrawText(jumpNote, (int)(card.x + pad), (int)(rowY + rowGap * 5 + 6), noteFontSize, noteColor);

    char dashNote[64];
    snprintf(dashNote, sizeof(dashNote), "Dash: double-tap %s or %s",KeyToString(player->keyLeft), KeyToString(player->keyRight));
    DrawText(dashNote, (int)(card.x + pad), (int)(rowY + rowGap * 5 + 6 + noteFontSize + 8), noteFontSize, noteColor);
}
static bool DrawBackButton(Rectangle rect)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bgColor     = hovered ? (Color){ 54, 46, 84, 255 }   : (Color){ 38, 32, 60, 255 };
    Color borderColor = hovered ? (Color){ 235, 225, 255, 255 } : (Color){ 150, 135, 190, 255 };
    Color textColor   = hovered ? (Color){ 235, 225, 255, 255 } : (Color){ 220, 215, 235, 255 };

    Rectangle drawRect = rect;

    DrawRectangleRounded(drawRect, 0.3f, 8, bgColor);
    DrawRectangleLinesEx(drawRect, 2.0f, borderColor);

    const char *label = "<  BACK";
    int fontSize = 20;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label,(int)(drawRect.x + (drawRect.width - textWidth) / 2.0f),(int)(drawRect.y + (drawRect.height - fontSize) / 2.0f),fontSize, textColor);

    return clicked;
}

bool DrawControlsScreen(Player *p1,Player *p2)
{
    ClearBackground((Color){ 22, 18, 38, 255 });
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    const char *title = "CONTROLS";
    int titleFontSize = 48;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (GetScreenWidth() - titleWidth) / 2, 60, titleFontSize, (Color){ 235, 225, 255, 255 });

    float cardWidth  = 480.0f;
    float cardHeight = 320.0f;
    float cardGap    = 40.0f;
    float cardsY     = 200.0f;
    float totalWidth = cardWidth * 2 + cardGap;
    float startX     = (GetScreenWidth() - totalWidth) / 2.0f;

    Rectangle leftCard  = { startX, cardsY, cardWidth, cardHeight };
    Rectangle rightCard = { startX + cardWidth + cardGap, cardsY, cardWidth, cardHeight };

    DrawPlayerCard(leftCard,  p1, "Player 1");
    DrawPlayerCard(rightCard, p2, "Player 2");

    /* Boton Back: esquina inferior izquierda, mismo margen que usan las tarjetas */
    float backWidth  = 140.0f;
    float backHeight = 48.0f;
    float backMargin = 30.0f;
    Rectangle backButton = {
        backMargin,
        GetScreenHeight() - backHeight - backMargin,
        backWidth,
        backHeight
    };

    return DrawBackButton(backButton);
}