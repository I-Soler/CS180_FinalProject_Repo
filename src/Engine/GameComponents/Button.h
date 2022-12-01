#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <AEX.h>


namespace AEX
{
	class Button : public LogicComp
	{
		AEX_RTTI_DECL(Button, LogicComp);

	public:
		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

	private:
		TransformComp* tr;
		Camera* cam;
		SceneHandler* handler;

		AEX::Input* input;
	};
}