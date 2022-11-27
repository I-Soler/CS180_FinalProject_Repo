#pragma once
#include <Core/AEXCore.h>
#include"Logic/AEXLogicSystem.h"
#include "PressPuzzleController.h"

namespace AEX
{
	class PressPuzzleController;
	struct CollisionStayEvent;
	class PressurePlate : public LogicComp
	{
		AEX_RTTI_DECL(PressurePlate, LogicComp);

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

		void Pressed(const CollisionStayEvent& collision);
		void ResetPressurePlate();
		void SetEndState();
		char id_[1] = { '0' };
		bool pressed = false;

		PressPuzzleController* pressPuzzle;

		//dani debug
		void PrintDebugState();
	private:
		char receiver_[32] = "Default";
	};

	struct PressurePlateEvent : public Event
	{
		PressurePlateEvent(std::string receiver) : receiver_(receiver) {}
		std::string receiver_;
	};
}

