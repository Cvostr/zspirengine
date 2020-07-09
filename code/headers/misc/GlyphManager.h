#ifndef zs_glyphmanager
#define zs_glyphmanager

#include <map>
#include <string>

#include "../render/zs-math.h"
#include "../engine/EngineManager.h"
#include "../render/zs-shader.h"
#include "ft2build.h"

#include FT_FREETYPE_H

class GlyphManager;

class CharacterGlyph{
private:

public:
    ZSVECTOR2 glyph_size;
    ZSVECTOR2 glyph_bearing;
    ZSVECTOR2 glyph_advance;

    unsigned char* texture_buffer;
    unsigned int gl_texture_id;

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

    void DrawChar(int _char, ZSVECTOR2 pos, unsigned int* char_length, ZSRGBCOLOR color);
    void DrawString(int* string, unsigned int len, ZSVECTOR2 pos, ZSRGBCOLOR color = ZSRGBCOLOR(255,255,255));
};

class GlyphManager : public EngineComponentManager{
private:
    FT_Library ftlib;
    std::vector<GlyphFontContainer*> fonts;
public:

    void addFontContainer(GlyphFontContainer* ptr); //add font container to vector

    FT_Library getFreetypeLibraryInstance();
    GlyphManager();
};

#endif
