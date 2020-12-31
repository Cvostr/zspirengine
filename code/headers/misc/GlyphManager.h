#pragma once

#include <map>
#include <string>

#include "../render/Math.hpp"
#include "../engine/EngineComponent.h"
#include "../render/Shader.hpp"
#include "../render/Texture.h"
#include "ft2build.h"

#include FT_FREETYPE_H

class GlyphManager;

class CharacterGlyph{
private:

public:
    Vec2 glyph_size;
    Vec2 glyph_bearing;
    Vec2 glyph_advance;

    Engine::Texture* texture;
    unsigned char* texture_buffer;

    CharacterGlyph();
};

class GlyphFontContainer{
private:
    FT_Face font;
    std::map<unsigned int, CharacterGlyph*> characters;
    GlyphManager* manager_ptr;
public:
    GlyphFontContainer(unsigned char* data, unsigned int bsize, unsigned int size, GlyphManager* manager);
    ~GlyphFontContainer();

    void loadGlyphs();
    void loadGlyph(unsigned int index);

    void DrawChar(int _char, Vec2 pos, unsigned int* char_length, ZSRGBCOLOR color);
    void DrawString(const char32_t* string, unsigned int len, Vec2 pos, ZSRGBCOLOR color = ZSRGBCOLOR(255,255,255));
};

class GlyphManager : public IEngineComponent {
private:
    FT_Library mFtlib;
    std::vector<GlyphFontContainer*> mFonts;
public:

    void addFontContainer(GlyphFontContainer* ptr); //add font container to vector

    FT_Library getFreetypeLibraryInstance();
    GlyphManager();
};
