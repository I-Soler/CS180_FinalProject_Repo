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

		void Shoot();

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		AEXTimer timer;
		TransformComp* ParentTr;

		AEVec2 bulletPos;
		AEVec2 bulletDir;
		float shootDelay = 0.0f;

		float Recharge = 16.0f;

		bool firstShot;
	};
}