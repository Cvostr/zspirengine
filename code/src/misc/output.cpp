#include "../../headers/misc/output.hpp"

using namespace Engine;

OutputManager::OutputManager() {
	showDialog = nullptr;
	consoleLogWorking = false;
	RE_TYPE = RuntimeErrorType::RE_TYPE_NONE;
}

void OutputManager::addConsoleLog(LogEntryType log_type, std::string message, ScriptErrorType err_type) {
	if (!consoleLogWorking) return;
	
	LogEntry entry;
	entry.log_type = log_type;
	entry.message = message;
	entry.error_type = err_type;

	this->entries.push_back(entry);
}

void OutputManager::addConsoleLog(LogEntry entry) {
	if (!consoleLogWorking) 
		return;
	this->entries.push_back(entry);
}

void OutputManager::clearMessagesWithType(LogEntryType log_type) {
	while (hasAnyMsgsWithType(log_type)) {
		for (unsigned int i = 0; i < entries.size(); i++) { //Iterating over all objects
			LogEntry* ls = &entries[i];
			if (ls->log_type == log_type) {
				for (unsigned int obj_i = i + 1; obj_i < entries.size(); obj_i++) { //Iterate over all next chidren
					entries[obj_i - 1] = entries[obj_i]; //Move it to previous place
				}
				entries.resize(entries.size() - 1); //Reduce vector length
				break;
			}
		}
	}
}

bool OutputManager::hasAnyMsgsWithType(LogEntryType re_type) {
	for (unsigned int i = 0; i < entries.size(); i++) { //Iterating over all objects
		LogEntry* ls = &entries[i];
		if (ls->log_type == re_type)
			return true;
	}
	return false;
}

void OutputManager::spawnRuntimeError(RuntimeErrorType re_type) {
	RE_TYPE = re_type;
}
void OutputManager::unsetRuntimeError() {
	RE_TYPE = RuntimeErrorType::RE_TYPE_NONE;
}