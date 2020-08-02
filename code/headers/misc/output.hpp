#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "../engine/EngineManager.h"
#include <string>
#include <vector>

enum class LogEntryType {
	LE_TYPE_ENGINE_MESSAGE,
	LE_TYPE_SCRIPT_MESSAGE,
	LE_TYPE_SCRIPT_ERROR
};

enum class ScriptErrorType {
	SE_TYPE_NONE,
	SE_TYPE_ERROR,
	SE_TYPE_WARNING
};

typedef struct LogEntry {
	LogEntryType log_type;
	//if it is a script error
	ScriptErrorType error_type;

	std::string message;
}LogEntry;

namespace Engine {

	class OutputManager : public EngineComponentManager {
	public:

		std::vector<LogEntry> entries;

		bool consoleLogWorking;
		void (*showDialog)(std::string, std::string);
		void addConsoleLog(LogEntryType log_type, std::string message, ScriptErrorType err_type = ScriptErrorType::SE_TYPE_NONE);

		OutputManager();
	};
}
#endif