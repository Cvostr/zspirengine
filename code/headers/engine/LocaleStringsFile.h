#ifndef LOCALE_STRINGS_FILE
#define LOCALE_STRINGS_FILE

#include <string>

#define LANGS_COUNT 6

namespace ZSPIRE {
	class LocaleStringsFile {
	private:

		bool readFileHeader(char* data);

		unsigned int StringsCount;
		unsigned int langsCount;
		std::string lang_codes[LANGS_COUNT];
	public:

		unsigned int getLanguagesCount();
		unsigned int getStringsCount();
		void pushLanguageCodeStr(std::string str);

		LocaleStringsFile(char* data);
		LocaleStringsFile(std::string file_path);
		~LocaleStringsFile();
	};
}

#endif