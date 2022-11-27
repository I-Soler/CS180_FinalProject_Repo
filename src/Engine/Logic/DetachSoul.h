#pragma once
#include <Core/AEXCore.h>
#include "../Composition/TransformComponent.h"
#include "AEXLogicSystem.h"
#include "../StateMachine/StateMachine.h"

namespace AEX
{
	struct CollisionEnterEvent;
	class DetachSoul : public LogicComp
	{
		AEX_RTTI_DECL(DetachSoul, LogicComp);

	public:

		void AddToSystem() override;
		void RemoveFromSystem() override;

		void Initialize() override;
		//void OnCreate() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void OnCollisionStarted(const CollisionEnterEvent& e);

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

	private:
		GameObject* soul;
	};
}