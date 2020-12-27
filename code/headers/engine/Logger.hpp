#pragma once

#include <string>
#include <type_traits>
#include <stdio.h>

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

	
		OpLogger& operator<<(int var) {
			printf("%i", var);
			return *this;
		}
		OpLogger& operator<<(unsigned int var) {
			printf("%u", var);
			return *this;
		}
		OpLogger& operator<<(float var) {
			printf("%f", var);
			return *this;
		}
		OpLogger& operator<<(const char* var) {
			printf("%s", var);
			return *this;
		}
		OpLogger& operator<<(std::string var) {
			printf("%s", var.c_str());
			return *this;
		}
		
	};
}