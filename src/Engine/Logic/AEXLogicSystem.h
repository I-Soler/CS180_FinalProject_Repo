#pragma once
#include <Core/AEXCore.h>
#include <Composition/AEXComponent.h>
#include <vector>
#include <thread>

namespace AEX {


	class LogicComp : public IComp
	{
		AEX_RTTI_DECL(LogicComp, IComp);
	public:
		void AddToSystem() override;
		void RemoveFromSystem() override;
	};

	struct logic_thread_info
	{
		LogicComp* thisPtr = nullptr;
		bool* timeToJoinAll = nullptr;
		bool timeToJoinThis = false;
	};
	class LogicSystem : public IBase
	{
		AEX_RTTI_DECL(LogicSystem, IBase);
		AEX_SINGLETON(LogicSystem) {}

	public: 
		bool Initialize() { return true; }
		void AddComp(LogicComp* comp);
		void RemoveComp(LogicComp* comp);
		void Update();
		void Shutdown();	// join all threads

		// a thread assigned to each LogicComp
		std::map<LogicComp*, std::thread> logic_thread_ids;
		std::map<LogicComp*, logic_thread_info> logic_thread_infos;
		bool timeToJoinSystem = false;

		std::vector<LogicComp*> mComponents;
	};

	void LogicUpdate(logic_thread_info*);
}

#define aexLogic AEX::Singletons::Get<AEX::LogicSystem>()