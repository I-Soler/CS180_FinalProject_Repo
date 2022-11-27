#pragma once

#include "StateMachine.h"
#include <Graphics/Components/AEXGfxComps.h>

namespace AEX
{
	struct PlayerHorizontalState : public State
	{
		AEX_RTTI_DECL(PlayerHorizontalState, State);

		virtual void OnCreate() override;
		virtual void LogicEnter() override;
		virtual void LogicUpdate() override;
		virtual void LogicExit() override;
		//virtual void Shutdown() override;	//not needed yet

		void SetSpeed(f32 speed);

		// class variables
	private:
		TransformComp* mPlayerTr;
		Renderable* mPlayerRen;
		f32 mSpeed;
		int mDir;
	};

	struct PlayerVerticalUpState : public State
	{
		AEX_RTTI_DECL(PlayerVerticalUpState, State);

		virtual void OnCreate() override;
		virtual void LogicEnter() override;
		virtual void LogicUpdate() override;
		virtual void LogicExit() override;
		//virtual void Shutdown() override;	//not needed yet

		void SetSpeed(f32 speed);

		// class variables
	private:
		TransformComp* mPlayerTr;
		Renderable* mPlayerRen;
		f32 mSpeed;
	};

	struct PlayerVerticalDownState : public State
	{
		AEX_RTTI_DECL(PlayerVerticalDownState, State);

		virtual void OnCreate() override;
		virtual void LogicEnter() override;
		virtual void LogicUpdate() override;
		virtual void LogicExit() override;
		//virtual void Shutdown() override;	//not needed yet

		void SetSpeed(f32 speed);

		// class variables
	private:
		TransformComp* mPlayerTr;
		Renderable* mPlayerRen;
		f32 mSpeed;
	};

	struct PlayerIdleState : public State
	{
		AEX_RTTI_DECL(PlayerIdleState, State);

		virtual void OnCreate() override;
		virtual void LogicUpdate() override;

	private:
		Renderable* mPlayerRen;
	};

	struct DummyState : public State
	{
		AEX_RTTI_DECL(DummyState, State);
	};

	/* SPINE STATE */
	/*
	struct AnimState : public State
	{
		AEX_RTTI_DECL(AnimState, State);

		SkeletonAnimComp* mpAnimComp = nullptr;
		bool				mbLoop = false;
		std::string			mAnimName;
		std::unordered_map<std::string, f32> mAnimMixValues;

		void InternalEnter() override
		{

			if (!mpAnimComp)
				return;

			mpAnimComp->SetCurrentAnimation(mAnimName.c_str(), mbLoop);

			// set the start time
			// specify if it should change automatically when the animation is done

			// call the base internal Enter
			State::InternalEnter();
		}
		void InternalUpdate()override
		{
			State::InternalUpdate();
		}
		void InternalExit()override
		{
			// get the next state
			if (auto nextState = dynamic_cast<AnimState*>(parentSM->mNextState->mNextState))
			{
				auto toAnimName = nextState->mAnimName;
				auto it mAnimMixValues.find(toAnimName);
				if (it != mAnimMixValue.end())
					mpAnimComp->SetMixValue(mAnimName.c_str(), toAnimName.c_str(), 0.5f);
			}
			State::InternalExit();
		}

		void StreamWrite(const nlohmann::json& j) override
		{
			mAnimName = j["animname"];
			mbLoop = j["animLoop"];
			auto& mixValues = j["mixValues"];
			for (auto& tmp : mAnimMixValues)
			{
				mAnimMixValues[tmp["animName"]] = tmp["mixValues"];
			}
		}
		void StreamRead(const nlohmann::json& j) override
		{
			j["animname"] = mAnimName;
			j["animLoop"] = mbLoop;
			auto& mixValues = j["mixValues"];
			for (auto mv : mAnimMixValues)
			{
				nlohmann::json tmp;
				tmp["animName"] = mv.first;
				tmp["mixVal"] = mv.second;
				mixValues.push_back(tmp);
			}
		}
	};

	struct Fighter_Idle : public AnimState
	{
		AEX_RTTI_DECL(Fighter_Idle, AnimState);

		Fighter_Idle()
		{
			mAnimName = "1_/idle active";
			mbLoop = true;
		}
		void Update()
		{
			if (aexInput->KeyPressed('A') || aexInput->KeyPressed('D'))
				parentSM->ChangeState("Move");
		}
	};

	class FighterController : public IComp
	{
		AEX_RTTI_DECL(FighterController, IComp);

		void OnCreate() override
		{
			auto state = new Fighter_Idle;
			animComp->SetSkin("black");
			state->mpAnimComp = GetOwner->GetComp<SkeletonAnimComp>();
			state->mAnimMixValues["1_/walk fight pose..."] = 1.5f;

			mBrain[0].AddState("Idle,", state);
			mBrain[0].SetInitialState("Idle");

			animComp->SetMixValue("1_/walk_fight_pose", "1_/idle active", 0.25f);
		}
	};

	void CreateFighter()
	{
		auto obj = CreateSpineObject(sSpineDatas["stick-fighter"]);

	}*/
}