#ifndef LOCALE_STRINGS_FILE
#define LOCALE_STRINGS_FILE

#include <string>
#include <vector>

#define LANGS_COUNT 6

#define LANG_EN 1
#define LANG_DE 2
#define LANG_FR 3
#define LANG_ES 4
#define LANG_IT 5
#define LANG_RU 6


namespace ZSPIRE {

	class LocString {
	public:
		unsigned int ID; //Integer ID for string
		std::string str_ID; //String ID for string

		std::u32string STR[LANGS_COUNT];
	
		LocString();
		~LocString();
	};

	class LocaleStringsFile {
	private:

		bool readFileHeader(char* data);

		unsigned int StringsCount;
		unsigned int langsCount;
		unsigned int lang_codes[LANGS_COUNT];
		std::vector<LocString> strings;
	public:
		LocString* getString(unsigned int i);
		LocString* getString(std::string str_ID);
		LocString* getStringById(unsigned int id);

		void pushString(LocString str);
		unsigned int getLang(unsigned int i);
		unsigned int getLanguagesCount();
		unsigned int getStringsCount();
		void pushLanguageCode(unsigned int lang_code);

		LocaleStringsFile(char* data);
		LocaleStringsFile(std::string file_path);
		~LocaleStringsFile();
	};
}

#endif