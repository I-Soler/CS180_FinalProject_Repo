/*!
* \file		StateMachine.cpp
* \brief	Defines functions of State, StateMachine and SuperStateComp structures.
* \details	Defines functions of State, StateMachine and SuperStateComp structures
			to operate over state machine.
* \author	Iñaki Soler  i.soler@digipen.edu
* \date		04/05/2022
* \copyright Copyright DigiPen Institute Of Technology. All Rights Reserved
*/

#include "../Platform/AEXTime.h"
#include "StateMachine.h"
#include "AEXStateMachineSystem.h"
#include "PlayerStates.h"
#include <Debug\AEXDebug.h>			// Debug
#include "imgui/imgui.h"


// ----------------------------------------------------------------------------
// STATE MACHINE
// ----------------------------------------------------------------------------
namespace AEX
{

#pragma region// STATE

	State::State()
		: mTimeInState(0.0f), mSuperStateComp(nullptr)
	{}

	void State::InternalOnCreate()
	{
		OnCreate();
	}

	void State::InternalEnter()
	{
		mTimeInState = 0.0f;
		LogicEnter();	// player starts jumping
	}
	void State::InternalUpdate()
	{
		mTimeInState += aexTime->GetFrameTime();
		LogicUpdate();	// player on air
	}
	void State::InternalExit()
	{
		LogicExit();	// player has fallen
	}

	void State::InternalShutdown()
	{
		Shutdown();
	}
	
	
	#pragma endregion
	
	#pragma region// SUPER STATE COMPONENT

	SuperStateComp::SuperStateComp()
		: mCurrentState(nullptr), mNextState(nullptr), mInitialState(nullptr)
	{
		for (int i = 0; i < mStates.size(); i++)
		{
			mStates[i] = nullptr;
		}
	}

	void SuperStateComp::AddToSystem()
	{
		aexStateMachine->AddStateMachine(this);
	}
	void SuperStateComp::RemoveFromSystem()
	{
		aexStateMachine->RemoveStateMachine(this);
	}
	
	void SuperStateComp::OnCreate()
	{
		for (u32 i = 0; i < mStates.size(); ++i)
		{
			mStates[i]->mSuperStateComp = this;
			mStates[i]->InternalOnCreate();
		}
		mCurrentState = mInitialState;
	}
	void SuperStateComp::Update()
	{
		if (mStates.empty())	return;
		// change State
		if (mNextState != NULL && mCurrentState != mNextState)
		{
			mCurrentState->InternalExit();
			mCurrentState = mNextState;
			mCurrentState->InternalEnter();
		}

		mCurrentState->InternalUpdate();
	}
	void SuperStateComp::Shutdown()
	{
		for (u32 i = 0; i < mStates.size(); ++i)
			mStates[i]->Shutdown();
		RemoveFromSystem();
	}

	bool SuperStateComp::Edit()
	{
		for (u32 i = 0; i < mStates.size(); ++i)
		{
			if (ImGui::CollapsingHeader(mStates[i]->GetName()) && ImGui::Button("Delete state"))
			{
				RemoveState(mStates[i]);
				break;
			}
		}
		if (ImGui::CollapsingHeader("Create state"))
		{
			//AddState
			auto allTypes = Rtti::GetAllTypes();
			for (auto it = allTypes.begin(); it != allTypes.end(); ++it)
			{
				if (!it->second.IsDerivedFrom(State::TYPE()))
					continue;
				if (ImGui::Button(it->second.GetName().c_str()))
				{
					State* newState = (State*)aexFactory->Create(it->second.GetName().c_str());
					newState->SetName(it->second.GetName().c_str());
					AddState(newState);
					return true;
				}
			}
		}
		return true;
	}
	void SuperStateComp::StreamRead(const nlohmann::json& j)
	{
		const nlohmann::json& statesJson = j["states"];
		for (auto& tmp : statesJson)
		{
			const std::string stateType = tmp["state name"];
			State* s = nullptr;
			s = AddState(stateType.c_str());

			if (s != nullptr)
				s->OnCreate();
			else
				DebugPrint("State %s not found", stateType.c_str());
		}
		OnCreate();
	}
	void SuperStateComp::StreamWrite(nlohmann::json& j) const
	{
		nlohmann::json& statesJson = j["states"];
		for (std::vector<State*>::const_iterator it = mStates.begin(); it != mStates.end(); ++it)
		{
			nlohmann::json jState;
			jState["state name"] = (*it)->GetName();
			statesJson.push_back(jState);
		}
	}
	
	State* SuperStateComp::AddState(const char* stateName)
	{
		// check if exists
		State* newState = GetState(stateName);
		if (newState != nullptr) return newState;

		// create and add
		newState = (State*)aexFactory->Create(stateName);
		newState->SetName(stateName);
		AddState(newState);
	}
	State* SuperStateComp::AddState(State* newState)
	{
		// check if exists
		State* existingState = GetState(newState);
		if (existingState != nullptr) return existingState;

		// assign State
		newState->mSuperStateComp = this;
		mStates.push_back(newState);
		if (mInitialState == nullptr)
			mInitialState = newState;

		for (u32 i = 0; i < mStates.size(); ++i)
			mStates[i]->InternalOnCreate();
		if (mCurrentState == nullptr)
			mCurrentState = mInitialState;

		return newState;
	}

	void SuperStateComp::RemoveState(State* state)
	{
		State* removedState = GetState(state);		// State destructor
		if (removedState == nullptr) return;
	
		for (std::vector<State*>::iterator it = mStates.begin(); it != mStates.end(); ++it)
			if (*it == state)
			{
				(*it)->Shutdown();
				delete (*it);		// call State destructor
				mStates.erase(it);	// remove from list
				return;
			}
	}
	State* SuperStateComp::GetState(const char* stateName)
	{
		for (u32 i = 0; i < mStates.size(); ++i)
			if (std::string(mStates[i]->GetName()) == std::string(stateName))
				return mStates[i];
		return nullptr;
	}
	State* SuperStateComp::GetState(State* state)
	{
		for (u32 i = 0; i < mStates.size(); ++i)
			if (mStates[i] == state)
				return mStates[i];
		return nullptr;
	}
	void SuperStateComp::SetInitState(State* state)
	{
		mInitialState = GetState(state);	// make sure state exits in mStates, otherwise nullptr
	}
	void SuperStateComp::SetInitState(const char* stateName)
	{
		mInitialState = GetState(stateName);	// make sure state exits in mStates, otherwise nullptr
	}
	void SuperStateComp::ChangeState(State* state)
	{
		if (GetState(state) != nullptr)
			mNextState = state;
	}
	void SuperStateComp::ChangeState(const char* stateName)
	{
		State* state = GetState(stateName);
		if (state != nullptr)
			mNextState = state;
	}
	void SuperStateComp::Clear()
	{
		for (u32 i = 0; i < mStates.size(); ++i)
			delete mStates[i];		// call State destructors
	
		mStates.clear();	// empty list
	
		// Set pointers to NULL
		mCurrentState = nullptr;
		mNextState = nullptr;
		mInitialState = nullptr;
	}
	void SuperStateComp::Reset()
	{
		mCurrentState = nullptr;		// Set current state to NULL
		mNextState = mInitialState;		// Now next is inital, so restart
	}
	#pragma endregion
	
}