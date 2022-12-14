#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
#include <Platform/AEXTime.h>
#include "Turret.h"

namespace AEX
{
	class BulletComp : public LogicComp
	{
		AEX_RTTI_DECL(BulletComp, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		TurretComp* gun;
		TransformComp* tr;
		AEXTimer timer;
		AEVec2 dir;

		static std::list<TransformComp*> AllBullets;
	};
}
