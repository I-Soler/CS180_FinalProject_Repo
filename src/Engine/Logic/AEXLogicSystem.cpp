#include "AEXLogicSystem.h"
#include <Composition/AEXComponent.h>
#include <Core/AEXContainers.h>
namespace AEX {

	void LogicSystem::AddComp(LogicComp* comp)
	{
		// avoid duplicates
		RemoveComp(comp);
		mComponents.push_back(comp);
	}

	void LogicSystem::RemoveComp(LogicComp* comp)
	{
		FOR_EACH(cIt, mComponents)
		{
			if (*cIt == comp) {
				mComponents.erase(cIt);
				return;
			}
		}
	}

	void LogicSystem::Update()
	{
		// If a new component is created while on this loop, dont update it till next frame
		std::vector<AEX::LogicComp*> tempCompList= mComponents;	
		for (auto comp : tempCompList) {
			if (comp->GetOwner()->Enabled())
				comp->Update();
		}
	}
	void LogicComp::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void LogicComp::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
}