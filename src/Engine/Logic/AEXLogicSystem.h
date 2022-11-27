#pragma once
#include <Core/AEXCore.h>
#include <Composition/AEXComponent.h>
#include <vector>

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
	private:
		std::vector<LogicComp*> mComponents;
	};
}

#define aexLogic AEX::Singletons::Get<AEX::LogicSystem>()