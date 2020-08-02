#include "../../headers/misc/output.hpp"

using namespace Engine;

OutputManager::OutputManager() {
	showDialog = nullptr;
	consoleLogWorking = false;
}

void OutputManager::addConsoleLog(LogEntryType log_type, std::string message, ScriptErrorType err_type) {
	LogEntry entry;
	entry.log_type = log_type;
	entry.message = message;
	entry.error_type = err_type;

	this->entries.push_back(entry);
}