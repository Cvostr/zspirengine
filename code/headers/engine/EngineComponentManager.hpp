#pragma once

#include "EngineComponent.h"
#include <vector>

namespace Engine {
	class EngineComponentManager {
	private:
		std::vector<IEngineComponent*> components;
	public:
		void startManager(IEngineComponent* component);
		void updateResolution(int W, int H);
		void destroyAllManagers();
		unsigned int GetComponentsCount() { 
			return static_cast<unsigned int>(components.size()); }
		
		EngineComponentManager();
	};
}