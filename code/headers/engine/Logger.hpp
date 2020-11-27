#pragma once

#include <string>
#include <type_traits>
#include <stdio.h>
#include <iostream>

namespace Engine {

	enum class LogType{
		LOG_TYPE_NONE,
		LOG_TYPE_INFO,
		LOG_TYPE_WARN,
		LOG_TYPE_ERROR
	};

	class OpLogger;

	class Logger {
	private:
		static OpLogger mOpLogger;
	public:
		static void Log(const std::string& msg, LogType type = LogType::LOG_TYPE_INFO);
		static OpLogger& Log(LogType type = LogType::LOG_TYPE_INFO);
	};

	class OpLogger {
		
	public:
		OpLogger() {}

		template <typename T>
		OpLogger& operator<<(T var) {
			if (std::is_same<T, const char*>::value || std::is_same<T, char*>::value) {
				printf("%s", (const char*)var);
			}
			if (std::is_same<T, std::string>::value) {
				//std::string Str = var;
				//printf("%s", Str.c_str());
				std::cout << var;
			}
			if (std::is_same<T, int>::value) {
				printf("%i", (int)var);
			}
			return *this;
		}
	};
}