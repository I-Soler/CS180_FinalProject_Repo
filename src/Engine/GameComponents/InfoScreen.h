#pragma once
#include <Core/AEXCore.h>
#include"Logic/AEXLogicSystem.h"
#include "PressPuzzleController.h"

namespace AEX
{
	struct CollisionStayEvent;
	class InfoScreen : public LogicComp
	{
		AEX_RTTI_DECL(InfoScreen, LogicComp);

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

		void Show();
		void Hide();
		//dani debug
		void PrintDebugState();
	private:
		f32  scaley;
		bool hidden = false;
		std::string receiver_ = "Default";
	};

	struct InfoScreenEvent : public Event
	{
		InfoScreenEvent(std::string receiver) : receiver_(receiver) {}
		std::string receiver_;
	};
}
