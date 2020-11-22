#pragma once

#include <string>
#include <type_traits>

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
			if (std::is_same<T, std::string>::value) {
				Logger::Log((std::string)var, LogType::LOG_TYPE_NONE);
			}
			return *this;
		}
	};
}