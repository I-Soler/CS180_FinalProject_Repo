#pragma once
#include <Core/AEXCore.h>
#include"Logic/AEXLogicSystem.h"
#include "PlayerMovement.h"

namespace AEX
{
	struct CollisionStayEvent;
	class Key : public LogicComp
	{
		AEX_RTTI_DECL(Key, LogicComp);

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

		void KeyGrab(const CollisionStayEvent& collision);

		//dani debug
		void PrintDebugState();

		PlayerMovement* player;
	private:
		char receiver_[64] = "Default";
		std::string keyname;
	};

	struct KeyEvent : public Event
	{
		KeyEvent(std::string receiver) : receiver_(receiver) {}
		std::string receiver_;
	};
}