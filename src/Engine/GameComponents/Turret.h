#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
#include <Platform/AEXTime.h>

namespace AEX
{
	class TurretComp : public LogicComp
	{
		AEX_RTTI_DECL(TurretComp, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		AEXTimer timer;
		TransformComp* ParentTr;

		static AEVec2 lastBulletPos;
		static AEVec2 lastBulletDir;
	};
}