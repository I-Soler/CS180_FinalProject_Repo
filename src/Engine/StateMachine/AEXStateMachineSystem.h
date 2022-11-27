#pragma once
#include <Core/AEXCore.h>
#include "StateMachine.h"


namespace AEX {

	class StateMachineSystem : public IBase
	{
		AEX_RTTI_DECL(StateMachineSystem, IBase);
		AEX_SINGLETON(StateMachineSystem) {}

	public:
		void AddStateMachine(SuperStateComp* comp);
		void RemoveStateMachine(SuperStateComp* comp);
		void Update();
	private:
		std::vector<SuperStateComp*> mSuperStateComps;
	};
}

#define aexStateMachine AEX::Singletons::Get<AEX::StateMachineSystem>()