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
		for (auto comp : mComponents) {
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