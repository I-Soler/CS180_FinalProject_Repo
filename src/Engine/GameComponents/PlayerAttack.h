#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>
#include <Physics/RigidbodyComponent.h>
#include <Collisions/AEXCollisionSystem.h>

namespace AEX
{
	class PlayerAttack : public LogicComp
	{
		AEX_RTTI_DECL(PlayerAttack, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		Collider* attackCollider = nullptr;	// public for enemy event subscription

	private:
		void CreateChildWithGhostCollider();

		RigidbodyComp* attackImpulse = nullptr;
		AEVec2 thrustForce = AEVec2(5000.0f, 0.0f);
		f32 mCounter = 1.0f;
	};
}