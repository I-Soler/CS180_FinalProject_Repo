#include "AEXLogicSystem.h"
#include <Composition/AEXComponent.h>
#include <Core/AEXContainers.h>
namespace AEX {

	void LogicSystem::AddComp(LogicComp* comp)
	{
		// avoid duplicates
		//RemoveComp(comp);

		// create new thread for this logic update
		if (logic_thread_ids.find(comp) == logic_thread_ids.end())
		{
			mComponents.push_back(comp);

			logic_thread_info t_info;
			t_info.thisPtr = comp;
			t_info.timeToJoinAll = &timeToJoinSystem;
			logic_thread_infos[comp] = t_info;
			logic_thread_ids[comp] = std::thread(LogicUpdate, &logic_thread_infos[comp]);
		}
	}

	void LogicSystem::RemoveComp(LogicComp* comp)
	{
		FOR_EACH(cIt, mComponents)
		{
			if (*cIt == comp) {
				// join this LogicComp
				logic_thread_infos[comp].timeToJoinThis = true;
				logic_thread_ids[comp].join();
				logic_thread_ids.erase(comp);
				logic_thread_infos.erase(comp);

				mComponents.erase(cIt);
				return;
			}
		}
	}

	void LogicSystem::Update()
	{
		// threads
		//for (unsigned i = 0; i < mComponents.size(); ++i)
		//{
		//	logic_thread_ids[mComponents[i]].join();
		//}

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
		while (*logicInfo->timeToJoinAll == false && logicInfo->timeToJoinThis == false)
			if (logicInfo->thisPtr != nullptr)
				logicInfo->thisPtr->Update();
	}

	void LogicSystem::Shutdown()
	{
		timeToJoinSystem = true;
		// threads

		for (auto it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			logic_thread_ids[*it].join();
			logic_thread_infos.erase(*it);
			logic_thread_ids.erase(*it);
		}
	}
}