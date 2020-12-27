#include "../../headers/engine/Logger.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace Engine;
using namespace std;

#define INFO_PREFIX "{INFO} "
#define WARN_PREFIX "{WARNING} "
#define ERROR_PREFIX "{ERROR} "

OpLogger Logger::mOpLogger = OpLogger();

void SetConsoleColor(int color) {
#ifdef _WIN32
	HANDLE cout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cout_handle, color | BACKGROUND_INTENSITY);
#endif
}

void PrintText(std::string text) {
	printf(text.c_str());
}

void Logger::Log(const std::string& msg, LogType type) {

	switch (type) {
		case LogType::LOG_TYPE_INFO :
			PrintText(INFO_PREFIX);
			break;
		case LogType::LOG_TYPE_WARN:
			PrintText(WARN_PREFIX);
			break;
		case LogType::LOG_TYPE_ERROR:
			PrintText(ERROR_PREFIX);
			break;
	}
	
	SetConsoleColor(FOREGROUND_GREEN);
	PrintText(msg + "\n");
}

OpLogger& Logger::Log(LogType type) {
	switch (type) {
	case LogType::LOG_TYPE_INFO:
		PrintText(INFO_PREFIX);
		break;
	case LogType::LOG_TYPE_WARN:
		PrintText(WARN_PREFIX);
		break;
	case LogType::LOG_TYPE_ERROR:
		PrintText(ERROR_PREFIX);
		break;
	}
	return mOpLogger;
}