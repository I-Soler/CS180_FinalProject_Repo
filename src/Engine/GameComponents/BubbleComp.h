#pragma once
#include <thread>
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
#include "Turret.h"
#include <Collisions/AEXCollisionSystem.h>		// Collider, CollisionEnter


namespace AEX
{
	// maximum distance to join another bubble
	#define	MAX_DIST_SQ 62500.0f	// 250 x 250
	#define	MIN_RADIUS	10.0f
	#define	MAX_RADIUS	300.0f

	class BubbleComp;
	struct thread_info
	{
		// bubble info
		BubbleComp* thisPtr = nullptr;
		AEVec2 pos;
		float radius;

		// shot info
		AEVec2 origin, dir;
	};

	class BubbleComp : public LogicComp
	{
		AEX_RTTI_DECL(BubbleComp, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		// logic

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		void Die(const CollisionEnterEvent& collision);


		float mSpeed;
		float mDodgeTime = 0.5f;
		RigidbodyComp* mRgbd;
		TransformComp* mTr;

		// thread variables
		std::vector<std::thread> thread_ids;
		AEXTimer mTimer;
		bool dodgeMoving = false;
		float dodgeAngle = 0.0f;
		bool canJoin = true;	// sets to false if another bubble is going to join to this

		bool DieOnContact = false;

		// static interface
		static std::list<GameObject*> otherBubbles;
		static std::map<TurretComp*, bool> turrets;

		AEXTimer cooldown;
		bool multitheaded = true;
	};

	void Dodge(thread_info);
	void Pop(thread_info& ti, float miniRadius, AEVec2 newPosLeft, AEVec2 newPosRight);

}