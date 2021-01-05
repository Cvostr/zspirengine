#include "../../headers/misc/GlyphManager.h"
#include "../../headers/game.h"

#include <GL/glew.h>
#include <iostream>

extern ZSGAME_DATA* game_data;

GlyphManager::GlyphManager(){
    if (FT_Init_FreeType(&this->mFtlib))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

}

CharacterGlyph::CharacterGlyph(){

}

FT_Library GlyphManager::getFreetypeLibraryInstance(){
    return this->mFtlib;
}

void GlyphManager::addFontContainer(GlyphFontContainer* ptr){
    this->mFonts.push_back(ptr);
}

GlyphFontContainer::GlyphFontContainer(unsigned char* data, unsigned int bsize, unsigned int size, GlyphManager* manager){
    manager_ptr = manager;
    //Load font by freetype
    FT_New_Memory_Face(manager->getFreetypeLibraryInstance(), data, bsize, 0, &this->font);
    FT_Set_Pixel_Sizes(this->font, 0, size);
    mGlyphTexture = Engine::allocTexture();

    
    mGlyphTextureBuffer = new unsigned char[texSize * texSize];
    memset(mGlyphTextureBuffer, 0, texSize * texSize);

    unsigned int WorkX = 0;
    unsigned int WorkY = 0;
    unsigned int MaxY = 0;
    //Fill arrays and texture buffer
    loadGlyphs(WorkX, WorkY, MaxY);

    mGlyphTexture->LoadTextureFromBufferUByte(mGlyphTextureBuffer,
        texSize,
        texSize,
        Engine::TextureFormat::FORMAT_R);
}

GlyphFontContainer::~GlyphFontContainer() {
    FT_Done_Face(this->font);

    std::for_each(characters.begin(), characters.end(),
        [](std::pair<unsigned int, CharacterGlyph*> element) {
            // Accessing VALUE from element.
            delete element.second;
        });
}

void GlyphFontContainer::loadGlyphs(unsigned int& WorkX,
                                    unsigned int& WorkY,
                                    unsigned int& MaxY){
    //Common ASCII symbols
    for(unsigned int i = 0; i < 255; i ++){
        loadGlyph(i, WorkX, WorkY, MaxY);
    }
    //Cyrillic Russian letters
    for(unsigned int i = 1040; i < 1103; i ++){
        loadGlyph(i, WorkX, WorkY, MaxY);
    }
    //Additional Cyrillic Ukrainian letters
    loadGlyph(1168, WorkX, WorkY, MaxY);
    loadGlyph(1169, WorkX, WorkY, MaxY);
    loadGlyph(1031, WorkX, WorkY, MaxY);
    loadGlyph(1111, WorkX, WorkY, MaxY);
    loadGlyph(1028, WorkX, WorkY, MaxY);
    loadGlyph(1108, WorkX, WorkY, MaxY);
    loadGlyph(1110, WorkX, WorkY, MaxY);
    loadGlyph(1030, WorkX, WorkY, MaxY);
}

void GlyphFontContainer::loadGlyph(unsigned int index,
                                   unsigned int& WorkX,
                                   unsigned int& WorkY,
                                   unsigned int& MaxY){
    //use freetype to load char
    FT_Load_Char(this->font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);
    //Allocating new Character Glyph
    CharacterGlyph* character = new CharacterGlyph;
    character->mGlyphSize.X = static_cast<float>(font->glyph->bitmap.width);
    character->mGlyphSize.Y = static_cast<float>(font->glyph->bitmap.rows);

    character->mGlyphAdvance.X = static_cast<float>(font->glyph->advance.x);
    character->mGlyphAdvance.Y = static_cast<float>(font->glyph->advance.y);

    character->mGlyphBearing.X = static_cast<float>(font->glyph->bitmap_left);
    character->mGlyphBearing.Y = static_cast<float>(font->glyph->bitmap_top);
    //This code adds glyph to general font texture
    if (((texSize - WorkX) < character->mGlyphSize.X)) {
        WorkX = 0;
        WorkY += MaxY;
        MaxY = 0;
    }
    if (MaxY < character->mGlyphSize.Y)
        MaxY = character->mGlyphSize.Y;

    character->mGlyphTextureStart = Vec2(WorkX, WorkY);

    for (unsigned int _y = 0; _y < character->mGlyphSize.Y; _y++) {
        unsigned int Ycoord = (WorkY + _y) * texSize;
        for (unsigned int _x = 0; _x < character->mGlyphSize.X; _x++) {
            unsigned int Xcoord = Ycoord + WorkX + _x;
            mGlyphTextureBuffer[Xcoord] = font->glyph->bitmap.buffer[(int)(_y * character->mGlyphSize.X + _x)];
        }
    }
    WorkX += character->mGlyphSize.X;

    this->characters.insert(std::pair<unsigned int, CharacterGlyph*>(index, character));
}

void GlyphFontContainer::DrawChar(int _char, Vec2 pos, unsigned int* char_length, RGBAColor color){
    CharacterGlyph* glyph = this->characters.at(static_cast<unsigned int>(_char));
    *char_length = static_cast<unsigned int>(glyph->mGlyphBearing.X + glyph->mGlyphSize.X);

    game_data->pipeline->renderGlyph(glyph, static_cast<int>(pos.X), static_cast<int>(pos.Y - (glyph->mGlyphSize.Y - glyph->mGlyphBearing.Y)), static_cast<int>(glyph->mGlyphSize.X), static_cast<int>(glyph->mGlyphSize.Y), color);
}
void GlyphFontContainer::DrawString(const char32_t* string, unsigned int len, Vec2 pos, RGBAColor color){
    mGlyphTexture->Use(0);
    unsigned int xpos_offset = static_cast<unsigned int>(pos.X);
    for(unsigned int i = 0; i < len; i ++){
        unsigned int char_len = 0;
        DrawChar(string[i], Vec2(static_cast<float>(xpos_offset), pos.Y), &char_len, color);
        xpos_offset += char_len;
    }
}
