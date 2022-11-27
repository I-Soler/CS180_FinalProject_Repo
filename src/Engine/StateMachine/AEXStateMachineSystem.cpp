#include "AEXStateMachineSystem.h"

namespace AEX {

	void StateMachineSystem::AddStateMachine(SuperStateComp* comp)
	{
		// avoid duplicates
		RemoveStateMachine(comp);
		mSuperStateComps.push_back(comp);
	}

	void StateMachineSystem::RemoveStateMachine(SuperStateComp* comp)
	{
		FOR_EACH(cIt, mSuperStateComps)
		{
			if (*cIt == comp) {
				mSuperStateComps.erase(cIt);
				return;
			}
		}
	}

	void StateMachineSystem::Update()
	{
		for (auto comp : mSuperStateComps) {
			if (comp->GetOwner()->Enabled())
			comp->Update();
		}
	}
}