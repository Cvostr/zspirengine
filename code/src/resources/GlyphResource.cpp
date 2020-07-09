#include "../../headers/engine/resources.h"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::GlyphResource::GlyphResource() {
    this->resource_type = RESOURCE_TYPE_FONT;
    font_ptr = nullptr;
    this->loadInstantly = true;
}

Engine::GlyphResource::~GlyphResource() {
    delete font_ptr;
}

void Engine::GlyphResource::load() {
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;

        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;

        font_ptr = new GlyphFontContainer(request->data, request->size, 48, game_data->glyph_manager);
        font_ptr->loadGlyphs();

        delete[] request->data;
        delete this->request;
    }
}

Engine::GlyphResource* Engine::ResourceManager::getFontByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_FONT && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<GlyphResource*>(resource_ptr);
    }
    return nullptr;
}