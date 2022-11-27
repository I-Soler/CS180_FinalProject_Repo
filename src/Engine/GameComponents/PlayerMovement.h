#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>

namespace AEX
{
	class PlayerMovement : public LogicComp
	{
		AEX_RTTI_DECL(PlayerMovement, LogicComp);

	public:

		void AddToSystem() override;
		void RemoveFromSystem() override;

		//void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;
	};
}