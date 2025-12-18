#include "Text.h"

Text::Text() : baseSize(0) {
    gameFont = { 0 };
}

Text::~Text() {
    if (gameFont.texture.id > 0) {
        UnloadFont(gameFont);
    }
}

void Text::LoadFont(const char* path, int fontSize) {
    this->baseSize = fontSize;

    gameFont = LoadFontEx(path, fontSize, 0, 250);
}

void Text::Draw(int x, int y, const std::string& msg, Color color, int limit) const {

    if (gameFont.texture.id <= 0) return;

    std::string textToDraw = msg;
    if (limit > 0 && (int)msg.length() > limit) {
        textToDraw = msg.substr(0, limit);
    }

    Vector2 position = { (float)x, (float)y };
    float spacing = 2.0f;

    DrawTextEx(gameFont, textToDraw.c_str(), position, (float)baseSize, spacing, color);
}