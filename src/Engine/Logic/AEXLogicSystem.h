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

	class LogicSystem : public IBase
	{
		AEX_RTTI_DECL(LogicSystem, IBase);
		AEX_SINGLETON(LogicSystem) {}

	public: 
		bool Initialize() { return true; }
		void AddComp(LogicComp* comp);
		void RemoveComp(LogicComp* comp);
		void Update();

		std::vector<std::thread> logic_thread_ids;
		std::vector<logic_thread_info*> logic_thread_infos;
		static unsigned thread_idx;

		std::vector<LogicComp*> mComponents;
	};
	unsigned LogicSystem::thread_idx = 0;

	struct logic_thread_info
	{
		unsigned idx;
	};
	void LogicUpdate(logic_thread_info*);
}

#define aexLogic AEX::Singletons::Get<AEX::LogicSystem>()