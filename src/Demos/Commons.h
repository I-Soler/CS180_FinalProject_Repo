#pragma once
#include <Logic/AEXLogicSystem.h>

namespace AEX
{
	class Camera;
	class TransformComp;
}

class CamController : public AEX::LogicComp {
	AEX_RTTI_DECL(CamController, AEX::LogicComp);
	AEX::Camera* cam;

	AEX::TransformComp* transform;
public:
	void OnCreate() override;
	void Update() override;
};