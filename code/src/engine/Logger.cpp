#include "../../headers/engine/Logger.hpp"
#include <iostream>

using namespace Engine;
using namespace std;

#define INFO_PREFIX "\\cy{INFO} "
#define WARN_PREFIX "\\co{WARNING} "
#define ERROR_PREFIX "\\cr{ERROR} "

OpLogger Logger::mOpLogger = OpLogger();

void Logger::Log(const std::string& msg, LogType type) {
	string output = "";

	switch (type) {
		case LogType::LOG_TYPE_INFO :
			output = INFO_PREFIX;
			break;
		case LogType::LOG_TYPE_WARN:
			output = WARN_PREFIX;
			break;
		case LogType::LOG_TYPE_ERROR:
			output = ERROR_PREFIX;
			break;
	}

	output += msg + "\n";
	cout << output;
}

OpLogger& Logger::Log(LogType type) {
	string output = "";
	switch (type) {
	case LogType::LOG_TYPE_INFO:
		output = INFO_PREFIX;
		break;
	case LogType::LOG_TYPE_WARN:
		output = WARN_PREFIX;
		break;
	case LogType::LOG_TYPE_ERROR:
		output = ERROR_PREFIX;
		break;
	}
	cout << output;
	return mOpLogger;
}