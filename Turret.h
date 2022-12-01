#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Physics/RigidbodyComponent.h>		// RigidbodyComp
//#include <Collisions/AEXCollisionSystem.h>		// Collider, CollisionStayEvent
//#include <Graphics/Components/AEXGfxComps.h>	// Renderable, Texture

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

		// logic

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

	};
}