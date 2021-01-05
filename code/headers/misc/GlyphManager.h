#pragma once

#include <map>
#include <string>

#include "../render/Math.hpp"
#include "../engine/EngineComponent.h"
#include "../render/Shader.hpp"
#include "../render/Texture.h"
#include "ft2build.h"

#include FT_FREETYPE_H
#define texSize 2048

class GlyphManager;

class CharacterGlyph{
private:

public:
    Vec2 mGlyphSize;
    Vec2 mGlyphBearing;
    Vec2 mGlyphAdvance;
    Vec2 mGlyphTextureStart;

    CharacterGlyph();
};

class GlyphFontContainer{
private:
    unsigned char* mGlyphTextureBuffer;
    Engine::Texture* mGlyphTexture;


    FT_Face font;
    std::map<unsigned int, CharacterGlyph*> characters;
    GlyphManager* manager_ptr;
public:
    GlyphFontContainer(unsigned char* data, unsigned int bsize, unsigned int size, GlyphManager* manager);
    ~GlyphFontContainer();

    void loadGlyphs(unsigned int& WorkX,
                    unsigned int& WorkY,
                    unsigned int& MaxY);
    void loadGlyph(unsigned int index,
                    unsigned int& WorkX,
                    unsigned int& WorkY,
                    unsigned int& MaxY);

    void DrawChar(int _char, Vec2 pos, unsigned int* char_length, RGBAColor color);
    void DrawString(const char32_t* string, unsigned int len, Vec2 pos, RGBAColor color = RGBAColor(255,255,255));
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
