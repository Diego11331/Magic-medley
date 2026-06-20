#include "raylib.h"
#include "player.h"
#include <stdbool.h>

#define SCORES_MAX_VISIBLE_ROWS 6

static bool DrawBackButton(Rectangle rect)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bgColor     = hovered ? (Color){ 54, 46, 84, 255 }   : (Color){ 38, 32, 60, 255 };
    Color borderColor = hovered ? (Color){ 235, 225, 255, 255 } : (Color){ 150, 135, 190, 255 };
    Color textColor   = hovered ? (Color){ 235, 225, 255, 255 } : (Color){ 220, 215, 235, 255 };

    Rectangle drawRect = rect;
    if (hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        drawRect.x += 1.0f;
        drawRect.y += 1.0f;
        drawRect.width  -= 2.0f;
        drawRect.height -= 2.0f;
    }

    DrawRectangleRounded(drawRect, 0.3f, 8, bgColor);
    DrawRectangleLinesEx(drawRect, 2.0f, borderColor);

    const char *label = "<  BACK";
    int fontSize = 20;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label,
             (int)(drawRect.x + (drawRect.width - textWidth) / 2.0f),
             (int)(drawRect.y + (drawRect.height - fontSize) / 2.0f),
             fontSize, textColor);

    if (hovered) SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

    return clicked;
}

static void DrawCenteredText(const char *text, float centerX, float y, int fontSize, Color color)
{
    int width = MeasureText(text, fontSize);
    DrawText(text, (int)(centerX - width / 2.0f), (int)y, fontSize, color);
}

static float ClampF(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/*
 * scores: arreglo plano alternado [p1_s1, p2_s1, p1_s2, p2_s2, ...]
 * scoreCount: cantidad total de elementos en scores (sessions = scoreCount/2)
 * El cuerpo de la tabla tiene altura fija (SCORES_MAX_VISIBLE_ROWS filas) y
 * scrollea con la rueda del mouse cuando hay mas sesiones de las que entran.
 * Header y fila de totales quedan siempre fijos.
 * Devuelve true en el frame en que se hizo click en "Back".
 */
bool DrawScoresScreen(const int *scores, int scoreCount, Player *p1, Player *p2)
{
    static float scrollOffset = 0.0f;

    ClearBackground((Color){ 22, 18, 38, 255 });
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    const char *title = "SCORES";
    int titleFontSize = 48;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (GetScreenWidth() - titleWidth) / 2, 60, titleFontSize, (Color){ 235, 225, 255, 255 });

    int sessions = scoreCount / 2;

    float cardWidth  = 600.0f;
    float pad        = 24.0f;
    float headerH    = 40.0f;
    float rowH       = 34.0f;
    float bodyH      = SCORES_MAX_VISIBLE_ROWS * rowH;
    float totalsGap  = 14.0f;
    float totalsH    = 44.0f;
    float cardHeight = pad * 2 + headerH + bodyH + totalsGap + totalsH;

    float cardX = (GetScreenWidth() - cardWidth) / 2.0f;
    float cardY = 170.0f;
    Rectangle card = { cardX, cardY, cardWidth, cardHeight };

    DrawRectangleRounded(card, 0.06f, 8, (Color){ 38, 32, 60, 255 });
    DrawRectangleLinesEx(card, 2.0f, (Color){ 150, 135, 190, 255 });

    float innerWidth  = cardWidth - pad * 2;
    float colSessionW = innerWidth * 0.25f;
    float colPlayerW  = innerWidth * 0.375f;

    float xSession = cardX + pad;
    float xP1      = xSession + colSessionW;
    float xP2      = xP1 + colPlayerW;

    float cSession = xSession + colSessionW / 2.0f;
    float cP1      = xP1 + colPlayerW / 2.0f;
    float cP2      = xP2 + colPlayerW / 2.0f;

    float y = cardY + pad;
    DrawCenteredText("SESSION",  cSession, y, 18, (Color){ 150, 135, 190, 255 });
    DrawCenteredText("PLAYER 1", cP1,      y, 18, p1->color);
    DrawCenteredText("PLAYER 2", cP2,      y, 18, p2->color);

    y += headerH;
    DrawLineEx((Vector2){ cardX + pad, y - 6 }, (Vector2){ cardX + cardWidth - pad, y - 6 },
               1.0f, (Color){ 60, 52, 90, 255 });

    float bodyTop = y;
    Rectangle bodyRect = { cardX, bodyTop, cardWidth, bodyH };

    float contentH  = sessions > 0 ? sessions * rowH : rowH;
    float maxScroll = contentH > bodyH ? contentH - bodyH : 0.0f;

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, bodyRect)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) scrollOffset -= wheel * rowH * 1.5f;
    }
    scrollOffset = ClampF(scrollOffset, 0.0f, maxScroll);

    Color rowColor = (Color){ 220, 215, 235, 255 };
    int totalP1 = 0, totalP2 = 0;

    BeginScissorMode((int)bodyRect.x, (int)bodyRect.y, (int)bodyRect.width, (int)bodyRect.height);

    if (sessions == 0) {
        DrawCenteredText("No sessions yet", cardX + cardWidth / 2.0f, bodyTop + bodyH / 2.0f - 9, 18, rowColor);
    } else {
        for (int i = 0; i < sessions; i++) {
            int score1 = scores[i * 2];
            int score2 = scores[i * 2 + 1];
            totalP1 += score1;
            totalP2 += score2;

            float rowY = bodyTop + i * rowH - scrollOffset;
            if (rowY + rowH < bodyTop || rowY > bodyTop + bodyH) continue;

            if (i % 2 == 1) {
                Rectangle stripe = { cardX + pad, rowY, innerWidth, rowH };
                DrawRectangleRec(stripe, (Color){ 32, 27, 50, 255 });
            }

            DrawCenteredText(TextFormat("%d", i + 1), cSession, rowY + 6, 18, rowColor);
            DrawCenteredText(TextFormat("%d", score1), cP1,     rowY + 6, 18, rowColor);
            DrawCenteredText(TextFormat("%d", score2), cP2,     rowY + 6, 18, rowColor);
        }
    }

    EndScissorMode();

    if (maxScroll > 0.0f) {
        float barWidth = 4.0f;
        float barX = cardX + cardWidth - pad - barWidth;
        DrawRectangle((int)barX, (int)bodyTop, (int)barWidth, (int)bodyH, (Color){ 45, 38, 68, 255 });

        float thumbH = bodyH * (bodyH / contentH);
        if (thumbH < 20.0f) thumbH = 20.0f;
        float thumbY = bodyTop + (scrollOffset / maxScroll) * (bodyH - thumbH);
        DrawRectangle((int)barX, (int)thumbY, (int)barWidth, (int)thumbH, (Color){ 150, 135, 190, 255 });
    }

    y = bodyTop + bodyH + totalsGap;
    DrawLineEx((Vector2){ cardX + pad, y }, (Vector2){ cardX + cardWidth - pad, y },
               2.0f, (Color){ 150, 135, 190, 255 });
    y += 8;

    DrawCenteredText("TOTAL", cSession, y + 4, 20, (Color){ 235, 225, 255, 255 });
    DrawCenteredText(TextFormat("%d", totalP1), cP1, y + 4, 24, p1->color);
    DrawCenteredText(TextFormat("%d", totalP2), cP2, y + 4, 24, p2->color);

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