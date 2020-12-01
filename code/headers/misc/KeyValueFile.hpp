#pragma once

#include <string>
#include <vector>
#include "../render/Math.hpp"


namespace Engine {

	class KV_RECORD {
	public:
		size_t _typeid;
		std::string key;
	};

	template <typename T>
	class KV : public KV_RECORD {
	public:
		T value;

		KV() {
			_typeid = typeid(T).hash_code();
		}
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
				rec->key = key;
			}
			//Assign value
			rec->value = value;

			records.push_back(rec);
		}
		template <typename T>
		T getValue(std::string key) {
			KV<T>* rec = static_cast<KV<T>*>(findRecordByKey(key));
			return rec->value;
		}

		void saveAll();

		KeyValueFile();
		~KeyValueFile();
	};

	template<typename T>
	KV<T>* CastKV(KV_RECORD* record) {
		return static_cast<KV<T>*>(record);
	}

	template<typename T>
	T GetValue(KV_RECORD* record) {
		KV<T>* kvint = CastKV<T>(record);
		T value = kvint->value;
		return value;
	}

	template <typename T>
	T ASGetValue(KeyValueFile* Obj, std::string key) {
		return Obj->getValue<T>(key);
	}

	template <typename T>
	void ASSetValue(KeyValueFile* Obj, std::string key, T value) {
		return Obj->setValue<T>(key, value);
	}
}

