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
    texture = Engine::allocTexture();
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
}

GlyphFontContainer::~GlyphFontContainer() {
    FT_Done_Face(this->font);

    std::for_each(characters.begin(), characters.end(),
        [](std::pair<unsigned int, CharacterGlyph*> element) {
            // Accessing VALUE from element.
            delete element.second;
        });
}

void GlyphFontContainer::loadGlyphs(){
    //Common ASCII symbols
    for(unsigned int i = 0; i < 255; i ++){
        loadGlyph(i);
    }
    //Cyrillic Russian letters
    for(unsigned int i = 1040; i < 1103; i ++){
        loadGlyph(i);
    }
    //Additional Cyrillic Ukrainian letters
    loadGlyph(1168);
    loadGlyph(1169);
    loadGlyph(1031);
    loadGlyph(1111);
    loadGlyph(1028);
    loadGlyph(1108);
    loadGlyph(1110);
    loadGlyph(1030);
}

void GlyphFontContainer::loadGlyph(unsigned int index){
    //use freetype to load char
    FT_Load_Char(this->font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);
    //Allocating new Character Glyph
    CharacterGlyph* character = new CharacterGlyph;
    character->glyph_size.X = static_cast<float>(font->glyph->bitmap.width);
    character->glyph_size.Y = static_cast<float>(font->glyph->bitmap.rows);

    character->glyph_advance.X = static_cast<float>(font->glyph->advance.x);
    character->glyph_advance.Y = static_cast<float>(font->glyph->advance.y);

    character->glyph_bearing.X = static_cast<float>(font->glyph->bitmap_left);
    character->glyph_bearing.Y = static_cast<float>(font->glyph->bitmap_top);
    //Allocate image buffer
    character->texture_buffer = new unsigned char[font->glyph->bitmap.width * font->glyph->bitmap.rows];
    //Copy Image data
    for(unsigned int i = 0; i < font->glyph->bitmap.width * font->glyph->bitmap.rows; i ++){
        character->texture_buffer[i] = font->glyph->bitmap.buffer[i];
    }

    character->texture->LoadTextureFromBufferUByte(font->glyph->bitmap.buffer,
        static_cast<int>(font->glyph->bitmap.width),
        static_cast<int>(font->glyph->bitmap.rows),
        Engine::TextureFormat::FORMAT_R);

    this->characters.insert(std::pair<unsigned int, CharacterGlyph*>(index, character));
}

void GlyphFontContainer::DrawChar(int _char, Vec2 pos, unsigned int* char_length, ZSRGBCOLOR color){
    CharacterGlyph* glyph = this->characters.at(static_cast<unsigned int>(_char));
    *char_length = static_cast<unsigned int>(glyph->glyph_bearing.X + glyph->glyph_size.X);

    game_data->pipeline->renderGlyph(glyph->texture, static_cast<int>(pos.X), static_cast<int>(pos.Y - (glyph->glyph_size.Y - glyph->glyph_bearing.Y)), static_cast<int>(glyph->glyph_size.X), static_cast<int>(glyph->glyph_size.Y), color);
}
void GlyphFontContainer::DrawString(int* string, unsigned int len, Vec2 pos, ZSRGBCOLOR color){
    unsigned int xpos_offset = static_cast<unsigned int>(pos.X);
    for(unsigned int i = 0; i < len; i ++){
        unsigned int char_len = 0;
        DrawChar(string[i], Vec2(static_cast<float>(xpos_offset), pos.Y), &char_len, color);
        xpos_offset += char_len;
    }
}
