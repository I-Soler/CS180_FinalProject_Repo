#include "PlayerMovement.h"
#include <Platform/AEXInput.h>
#include "imgui/imgui.h"
#include "../StateMachine/PlayerStates.h"

namespace AEX
{
	void PlayerMovement::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void PlayerMovement::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void PlayerMovement::Initialize()
	{
		SuperStateComp* mSuperState = mOwner->GetComp<SuperStateComp>();

		// create necessary SuperStateComp
		if (mSuperState == nullptr)
		{
			mSuperState = mOwner->NewComp<SuperStateComp>();
			mSuperState->AddToSystem();
			mSuperState->OnCreate();
			mSuperState->Initialize();
		}
		// add neccessary states (nothing happens if the exist)
		mSuperState->AddState("PlayerIdleState");
		mSuperState->AddState("PlayerVerticalUpState");
		mSuperState->AddState("PlayerVerticalDownState");
		mSuperState->AddState("PlayerHorizontalState");
	}
	bool PlayerMovement::Edit()
	{
		ImGui::Text("No variables to set");
		return true;
	}
	void PlayerMovement::Update()
	{

	}
	void PlayerMovement::Shutdown()
	{
		RemoveFromSystem();
	}

	void PlayerMovement::StreamRead(const nlohmann::json& j)
	{
		//mSpeed = j["speed"];
	}
	void PlayerMovement::StreamWrite(nlohmann::json& j) const
	{
		//j["speed"] = mSpeed;
	}
}