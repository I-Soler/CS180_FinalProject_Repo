#pragma once
#include <Core/AEXCore.h>
#include"Logic/AEXLogicSystem.h"
#include "PressurePlate.h"

namespace AEX
{
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

		void DoorActivated(const PressurePlateEvent& event);
	};
}
