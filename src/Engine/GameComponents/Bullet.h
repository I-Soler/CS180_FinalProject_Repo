#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
#include <Platform/AEXTime.h>

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

		TransformComp* tr;
		AEXTimer timer;
	};
}
