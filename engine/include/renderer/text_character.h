#pragma once

#include "emath/emath.h"

struct TextCharacter {
    int textureID;          // ID handle of the glyph texture
    Math::Vector2D   size;      // Size of glyph
    Math::Vector2D   bearing;   // Offset from baseline to left/top of glyph
    unsigned int advance;   // Horizontal offset to advance to next glyph
};