#pragma once
#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>
#include <Collisions/AEXCollisionSystem.h>

namespace AEX
{
	class EnemyHealth : public LogicComp
	{
		AEX_RTTI_DECL(EnemyHealth, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		// logic
		void GetHurt(const CollisionStayEvent&);

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

	private:
		RigidbodyComp* enemyPhy;
		Collider* enemyBox = nullptr;
		f32 enemyHealth = 100.0f;
	};
}