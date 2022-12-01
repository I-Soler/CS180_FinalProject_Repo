#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Turret.h"
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	void TurretComp::OnCreate()
	{
	}
	void TurretComp::Initialize()
	{
	}
	void TurretComp::Update()
	{
	}
	void TurretComp::Shutdown()
	{
		RemoveFromSystem();
	}
	bool TurretComp::Edit()
	{
		return false;
	}
	void TurretComp::StreamRead(const nlohmann::json& j)
	{
	}
	void TurretComp::StreamWrite(nlohmann::json& j) const
	{
	}
}