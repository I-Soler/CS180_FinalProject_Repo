#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

//#include <GAM150\SpriteAnimation.h>
//#include <GAM150\Sprite.h>
#include <string>
//#include <map>
#include "../Composition/AEXComposition.h"

namespace AEX
{
	struct State;

	struct SuperStateComp : public IComp
	{
		AEX_RTTI_DECL(SuperStateComp, IComp);
		std::vector<State*> mStates;

		State* mCurrentState;
		State* mNextState;
		State* mInitialState;

		SuperStateComp();	// better in Initialize()

		void AddToSystem() override;
		void RemoveFromSystem() override;

		virtual void OnCreate();
		virtual void Update();
		virtual void Shutdown();
		bool Edit()override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		State* AddState(const char* stateName);
		State* AddState(State*);
		State* AddState(const char* stateName, State*);
		void RemoveState(State* state);
		State* GetState(const char* stateName);
		State* GetState(State* state);
		void SetInitState(State* state);
		void SetInitState(const char* stateName);
		void ChangeState(State* state);
		void ChangeState(const char* stateName);
		void Clear();
		void Reset();
	};

struct State : IBase
{
	AEX_RTTI_DECL(State, IBase);

	State();
	virtual ~State() {}

	virtual void OnCreate(){}
	virtual void LogicEnter(){}
	virtual void LogicUpdate(){}
	virtual void LogicExit(){}
	virtual void Shutdown(){}

	// basic loop for all States
	virtual void InternalOnCreate();
	virtual void InternalEnter();
	virtual void InternalUpdate();
	virtual void InternalExit();
	virtual void InternalShutdown();

	SuperStateComp* mSuperStateComp;			// SuperStateComp component that holds this State
	f64 mTimeInState;							// how long this state has been active
	std::map<std::string, State*> mNextStates;
};


// @PROVIDED
//! \class Actor
//! \brief Simple GameObject structure that supports state machines. 
////! \details Feel free to use this structure in your demos.  
//struct Actor : public Sprite
//{
//	//! Use a vector to support concurrent state machine
//	std::vector<StateMachine> mBrain;
//
//	//! Adds one state machine to be used by default.
//	Actor() :Sprite()
//	{
//		// add default state machine
//		mBrain.push_back(StateMachine(this));
//	}
//
//	//! Updates all state machine sequentially.
//	virtual void Update()
//	{
//		for (u32 i = 0; i < mBrain.size(); ++i)
//			mBrain[i].Update();
//
//		Sprite::Update();
//	}
//
//	// @SUGGESTION
//	/*! Add extra functionality to easily manage the actor's state machines
//		and their respective states. For example, the following functions could be 
//		useful:
//
//		State * GetState(stateMachineName, stateName);
//		State * AddState(stateMachineName, newState);
//		void  ChangeState(stateMachineName, nextState);
//
//		State * AddState(newState); // adds to default state machine.
//		State * GetState(stateName); // gets state from default state machine.
//		boid ChangeState(nextState); // changes state in default state machine. 
//
//		etc...
//	*/
//};

// @PROVIDED
//! \class	AnimState
//! \brief	Basic state that overrides the internal enter to automatically 
//!			change the animation on a Sprite object. 
//! \details Feel free to use this structure in your demos.
//struct AnimState : public State
//{
//	Spritesheet	*			mSpriteSheet;
//	SpriteAnimationData *	mAnimation;
//	u32						mInitialFrame;
//	f32						mAnimSpeed;
//	bool					mLoop;
//
//	AnimState(const char * name)
//		: State(name)
//		, mSpriteSheet(0)
//		, mAnimation(0)
//		, mInitialFrame(0)
//		, mAnimSpeed(1.0f)
//		, mLoop(false)
//	{}
//
//	// internal versions
//	void InternalEnter()
//	{
//		if (mAnimation) {
//			// set the animation state data
//			Sprite * sprite = (Sprite*)mActor;
//			sprite->SetAnimation(sprite->mpAnimData);
//			sprite->SetLoop(mLoop);
//			sprite->mAnimSpeed = mAnimSpeed;
//			sprite->SetAnimationFrame(mInitialFrame);
//		}
//		else if (mSpriteSheet){
//			// set animation state
//			Sprite * sprite = (Sprite *)mActor;
//			sprite->SetSpritesheet(mSpriteSheet);
//			sprite->SetLoop(mLoop);
//			sprite->mAnimSpeed = mAnimSpeed;
//			sprite->SetAnimationFrame(mInitialFrame);
//		}
//		mActor->mbAnimPlaying = true;
//
//		// always call the base function to insure default functionality
//		State::InternalEnter();
//	}
//};
}

#endif