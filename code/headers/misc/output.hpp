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

enum class RuntimeErrorType {
	RE_TYPE_NONE,
	RE_TYPE_GL_CREATION_ERROR,
	RE_TYPE_SHADER_ERROR,
	RE_TYPE_SCRIPT_ERROR,
	RE_TYPE_SCENE_OPEN_ERROR
};

typedef struct LogEntry {
	LogEntryType log_type;
	//if it is a script error
	ScriptErrorType error_type;

	std::string message;

	LogEntry() {
		error_type = ScriptErrorType::SE_TYPE_NONE;
	}

}LogEntry;

namespace Engine {

	class OutputManager : public EngineComponentManager {
	public:
		RuntimeErrorType RE_TYPE;
		std::vector<LogEntry> entries;

		bool consoleLogWorking;
		void (*showDialog)(std::string, std::string);
		void addConsoleLog(LogEntryType log_type, std::string message, ScriptErrorType err_type = ScriptErrorType::SE_TYPE_NONE);
		void addConsoleLog(LogEntry entry);
		void spawnRuntimeError(RuntimeErrorType re_type);
		void unsetRuntimeError();
		void clearMessagesWithType(LogEntryType log_type);

		OutputManager();
	};
}
#endif