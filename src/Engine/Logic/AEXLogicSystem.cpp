#include "AEXLogicSystem.h"
#include <Composition/AEXComponent.h>
#include <Core/AEXContainers.h>
namespace AEX {

	unsigned LogicSystem::thread_idx = 0;

	void LogicSystem::AddComp(LogicComp* comp)
	{
		// avoid duplicates
		RemoveComp(comp);
		mComponents.push_back(comp);

		// create new thread for this logic update
		logic_thread_infos.push_back(new logic_thread_info);
		logic_thread_infos.back()->idx = thread_idx++;
		logic_thread_ids.push_back(std::thread(LogicUpdate, logic_thread_infos.back()));
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
		// threads

		// not threads
		//for (auto comp : mComponents) {
		//	if (comp->GetOwner()->Enabled())
		//		comp->Update();
		//}
	}
	void LogicComp::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void LogicComp::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}

	void LogicUpdate(logic_thread_info* logicInfo)
	{
		// multithreaded update
		aexLogic->mComponents[logicInfo->idx]->Update();
	}
}