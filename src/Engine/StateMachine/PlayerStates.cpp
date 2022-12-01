#include <AEX.h>
#include <AEX.h>
#include "PlayerStates.h"
#include <iostream>
//#include "StateMachine.h"
//#include <Debug/AEXDebug.h>

namespace AEX
{
	#pragma region// PLAYER HORIZONTAL STATE
	void PlayerHorizontalState::OnCreate()
	{
		mPlayerRen = mSuperStateComp->GetOwner()->GetComp<Renderable>();
		mPlayerTr = mSuperStateComp->GetOwner()->GetComp<TransformComp>();
		mSpeed = 1.0f;

		// add next possible States
		mNextStates["Idle"]			= mSuperStateComp->GetState("PlayerIdleState");
		mNextStates["VerticalUp"]	= mSuperStateComp->GetState("PlayerVerticalUpState");
		mNextStates["VerticalDown"]	= mSuperStateComp->GetState("PlayerVerticalDownState");
	}
	void PlayerHorizontalState::LogicEnter()
	{
		//mPlayerRen->SetModColor(Color(1, 0, 0, 1));	// red

		if (aexInput->KeyPressed('D'))
			mDir = 1;	// right
		else if (aexInput->KeyPressed('A'))
			mDir = -1;	// left
	}
	void PlayerHorizontalState::LogicUpdate()
	{
		// update position
		mPlayerTr->SetPosition(mPlayerTr->GetPosition() + AEVec3(1, 0, 0) * mSpeed * (f32)mDir);

		// change direction if opposite triggered
		if (aexInput->KeyTriggered('D') || aexInput->KeyTriggered('A'))
			mDir = -mDir;

		// diagonal up
		if (aexInput->KeyPressed('W'))
		{
			if (!aexInput->KeyPressed('D') && !aexInput->KeyPressed('A'))
				mSuperStateComp->mNextState = mNextStates["VerticalUp"];
			else
				mPlayerTr->SetPosition(mPlayerTr->GetPosition() + AEVec3(0, 1, 0) * mSpeed);
		}
		// diagonal down
		else if (aexInput->KeyPressed('S'))
		{
			if (!aexInput->KeyPressed('D') && !aexInput->KeyPressed('A'))
				mSuperStateComp->mNextState = mNextStates["VerticalDown"];
			else
				mPlayerTr->SetPosition(mPlayerTr->GetPosition() + AEVec3(0, -1, 0) * mSpeed);
		}

		// check if state finished
		else if (aexInput->KeyTriggered('S'))
			mSuperStateComp->mNextState = mNextStates["VerticalDown"];
		else if (!aexInput->KeyPressed('D') && !aexInput->KeyPressed('A'))
			mSuperStateComp->mNextState = mNextStates["Idle"];
	}
	void PlayerHorizontalState::LogicExit()
	{
		// reset variables
	}
	#pragma endregion



	#pragma region// PLAYER IDLE STATE
	void PlayerIdleState::OnCreate()
	{
		mPlayerRen = mSuperStateComp->GetOwner()->GetComp<Renderable>();

		// add next possible States
		mNextStates["Idle"]			= mSuperStateComp->GetState("PlayerIdleState");
		mNextStates["VerticalUp"]	= mSuperStateComp->GetState("PlayerVerticalUpState");
		mNextStates["VerticalDown"]	= mSuperStateComp->GetState("PlayerVerticalDownState");
		mNextStates["Horizontal"]	= mSuperStateComp->GetState("PlayerHorizontalState");
	}
	void PlayerIdleState::LogicUpdate()
	{
		//mPlayerRen->SetModColor(Color(1, 1, 1, 1));	// white

		// check if state finished
		if (aexInput->KeyPressed('A') || aexInput->KeyPressed('D'))
			mSuperStateComp->mNextState = mNextStates["Horizontal"];
		else if (aexInput->KeyPressed('S'))
			mSuperStateComp->mNextState = mNextStates["VerticalDown"];
		else if (aexInput->KeyPressed('W'))
			mSuperStateComp->mNextState = mNextStates["VerticalUp"];
	}
	#pragma endregion


	#pragma region// PLAYER VERTICAL UP STATE
	void PlayerVerticalUpState::OnCreate()
	{
		mPlayerRen = mSuperStateComp->GetOwner()->GetComp<Renderable>();
		mPlayerTr = mSuperStateComp->GetOwner()->GetComp<TransformComp>();
		mSpeed = 1.0f;

		// add next possible States
		mNextStates["Idle"]			= mSuperStateComp->GetState("PlayerIdleState");
		mNextStates["VerticalDown"]	= mSuperStateComp->GetState("PlayerVerticalDownState");
		mNextStates["Horizontal"]	= mSuperStateComp->GetState("PlayerHorizontalState");
	}
	void PlayerVerticalUpState::LogicEnter()
	{
		//mPlayerRen->SetModColor(Color(0, 1, 0, 1));	// green
	}
	void PlayerVerticalUpState::LogicUpdate()
	{
		// update position
		mPlayerTr->SetPosition(mPlayerTr->GetPosition() + AEVec3(0, 1, 0) * mSpeed);

		// check if state finished
		if (aexInput->KeyTriggered('A') || aexInput->KeyTriggered('D'))
			mSuperStateComp->mNextState = mNextStates["Horizontal"];
		else if (aexInput->KeyTriggered('S'))
			mSuperStateComp->mNextState = mNextStates["VerticalDown"];
		else if (!aexInput->KeyPressed('W'))
			mSuperStateComp->mNextState = mNextStates["Idle"];
	}
	void PlayerVerticalUpState::LogicExit()
	{
		// reset variables
	}
	#pragma endregion


	#pragma region// PLAYER VERTICAL DOWN STATE
	void PlayerVerticalDownState::OnCreate()
	{
		mPlayerRen = mSuperStateComp->GetOwner()->GetComp<Renderable>();
		mPlayerTr = mSuperStateComp->GetOwner()->GetComp<TransformComp>();
		mSpeed = 1.0f;

		// add next possible States
		mNextStates["Idle"]			= mSuperStateComp->GetState("PlayerIdleState");
		mNextStates["VerticalUp"]	= mSuperStateComp->GetState("PlayerVerticalUpState");
		mNextStates["Horizontal"]	= mSuperStateComp->GetState("PlayerHorizontalState");
	}
	void PlayerVerticalDownState::LogicEnter()
	{
		//mPlayerRen->SetModColor(Color(0, 0, 1, 1));	// blue
	}
	void PlayerVerticalDownState::LogicUpdate()
	{
		// update position
		mPlayerTr->SetPosition(mPlayerTr->GetPosition() - AEVec3(0, 1, 0) * mSpeed);

		// check if state finished
		if (aexInput->KeyTriggered('A') || aexInput->KeyTriggered('D'))
			mSuperStateComp->mNextState = mNextStates["Horizontal"];
		else if (aexInput->KeyTriggered('W'))
			mSuperStateComp->mNextState = mNextStates["VerticalUp"];
		else if (!aexInput->KeyPressed('S'))
			mSuperStateComp->mNextState = mNextStates["Idle"];
	}
	void PlayerVerticalDownState::LogicExit()
	{
		// reset variables
	}
	#pragma endregion

	void PlayerVerticalUpState::SetSpeed(f32 sp) { mSpeed = sp; }
	void PlayerVerticalDownState::SetSpeed(f32 sp) { mSpeed = sp; }
	void PlayerHorizontalState::SetSpeed(f32 sp) { mSpeed = sp; }
}