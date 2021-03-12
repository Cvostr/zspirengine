#pragma once

namespace Engine {

	typedef struct LocalizationSettings {
		unsigned int SelectedLocaleStringLanguage;

		LocalizationSettings() :
			SelectedLocaleStringLanguage(0)
		{}
	};

	typedef struct GraphicalSettings {
		unsigned int maxTextureAnisoLevel;

		int maxTextureMipLevel;
	};

	typedef struct EngineSettings {
		LocalizationSettings mLocalizationSettings;
		GraphicalSettings mGraphicalSettings;
	};

}