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
		
		EngineComponentManager();
	};
}