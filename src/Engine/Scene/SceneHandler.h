#pragma once

#include <Core/AEXCore.h>
#include "Logic/AEXLogicSystem.h"
#include <Platform/AEXTime.h>
#include "MessagingSystem/IEvent.h"

namespace AEX
{
	class SceneHandler : public LogicComp
	{
		AEX_RTTI_DECL(SceneHandler, LogicComp);

	public:
		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

		void ChangeScene();

		AEXTimer timer;
		bool ChangesWithTime;
		float TimeToChange;
		float TimeToFade;

		std::string ChangeToScene = "Empty";
	};
}
