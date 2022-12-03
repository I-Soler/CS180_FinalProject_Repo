#pragma once
#include <thread>
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
#include "Turret.h"
//#include <Collisions/AEXCollisionSystem.h>		// Collider, CollisionStayEvent
//#include <Graphics/Components/AEXGfxComps.h>	// Renderable, Texture

namespace AEX
{
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

		float mSpeed;
		RigidbodyComp* mRgbd;
		TransformComp* mTr;

		// thread variables
		std::vector<std::thread> thread_ids;
		//std::vector<thread_info> thread_infos;
		bool dodgeMoving = false;
		float dodgeAngle = 0.0f;

		static bool shotDone;
	};

	void Dodge(thread_info);

}