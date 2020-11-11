#ifndef FILES_AG_HPP
#define FILES_AG_HPP

#include <string>
#include <vector>
#include "../render/Math.hpp"


namespace Engine {

	class KV_RECORD {
	public:
		std::string key;
	};

	template <typename T>
	class KV : public KV_RECORD {
	public:
		T value;
	};

	class KeyValueFile {
	private:
		std::string file_path;
	public:
		void ReadFile(std::string path);
		std::vector<KV_RECORD*> records;

		KV_RECORD* findRecordByKey(std::string key);

		template <typename T>
		void setValue(std::string key, T value) {
			KV<T>* rec = static_cast<KV<T>*>(findRecordByKey(key));

			if (rec == nullptr) {
				rec = new KV<T>();
			}
			//Assign value
			rec->value = value;

			records.push_back(rec);
		}
		template <typename T>
		T getValue(std::string key) {
			KV<T>* rec = findRecordByKey(key);
			return rec->value;
		}

		void saveAll();

		KeyValueFile();
		~KeyValueFile();
	};
}

#endif