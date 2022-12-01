#pragma once
#include <Core/AEXCore.h>
#include"Logic/AEXLogicSystem.h"

namespace AEX
{
	struct CollisionStayEvent;
	class Door : public LogicComp
	{
		AEX_RTTI_DECL(Door, LogicComp);

	public:
		void AddToSystem() override;
		void RemoveFromSystem() override;

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		void DoorActivated(const CollisionStayEvent& collision);
	};

	struct DoorEvent : public Event
	{
		DoorEvent(std::string receiver) : receiver_(receiver) {}
		std::string receiver_;
	};
}
