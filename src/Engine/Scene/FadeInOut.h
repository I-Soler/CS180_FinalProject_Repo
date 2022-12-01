#pragma once

#include <Core/AEXCore.h>
#include "Logic/AEXLogicSystem.h"
#include "Graphics/Components/AEXGfxComps.h"

namespace AEX
{
	class Fade : public LogicComp
	{
		AEX_RTTI_DECL(Fade, LogicComp);

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

		bool fadingIn = true;
		bool fadingOut = false;

		Renderable* renderable;
	};
}
