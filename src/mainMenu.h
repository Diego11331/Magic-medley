#include "player.h"
#include "wands.h"

typedef enum ButtonId{
    BUTTON_PLAY,
    BUTTON_CONTROLS,
    BUTTON_SCORE,
    BUTTON_EXIT
}ButtonId;
typedef struct MenuButton{
    Rectangle  bounds;
    const char *label;
    ButtonId   id;
    bool       isHovered;
    bool       isPressed;
}MenuButton;
bool DrawMenuButton(MenuButton *btn, bool isPrimary,Color titleColor,Color subtitleColor);