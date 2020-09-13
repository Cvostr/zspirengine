#ifndef FILES_AG_HPP
#define FILES_AG_HPP

#include <string>
#include <vector>

namespace Engine {

	class KV_RECORD {
	public:
		std::string key;
	};

	class KV_INT : public KV_RECORD {
	public:
		int value;
	};

	class KV_FLOAT : public KV_RECORD {
	public:
		float value;
	};

	class KV_BOOL : public KV_RECORD {
	public:
		bool value;
	};

	class KV_STR : public KV_RECORD {
	public:
		std::string value;
	};

	class KeyValueFile {
	public:

		std::vector<KV_RECORD*> records;

		KV_RECORD* findRecordByKey(std::string key);

		template <typename T>
		void setValue(std::string key, T value) {
			KV_RECORD* rec = findRecordByKey(key);

			if (rec == nullptr) {
				switch (typeid(T)) {
					case typeid(float) :
						rec = new KV_INT;
						break;
					case typeid(int) :
						rec = new KV_INT;
						break;
				}
			}

			records.push_back(rec);
		}
		template <typename T>
		T getValue(std::string key);

		void saveAll();

		KeyValueFile();
		~KeyValueFile();
	};
}

#endif