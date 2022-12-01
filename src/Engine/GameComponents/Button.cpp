#include "Button.h"
#include "Collisions/Collisions.h"
#include "imgui/imgui.h"

namespace AEX
{
	void Button::OnCreate()
	{
	}
	void Button::Initialize()
	{
		input = aexInput;
		tr = GetOwner()->GetComp<TransformComp>();

		// Cambiar luego pa cojer por tag
		GameObject* camObj = GetOwnerSpace()->FindObjectByName("Camera");
		cam = camObj->GetComp <Camera>();

		handler = GetOwner()->GetComp<SceneHandler>();
	}
	void Button::Update()
	{
		if (!input->MousePressed(0))
			return;

		AEVec2 mousepos = aexInput->GetMousePos();
		cam->ViewToWorld(mousepos);

		AEVec2 pos(tr->GetPosition().x, tr->GetPosition().y + tr->GetScale().y / 2);

		if (StaticPointToStaticRect(&mousepos, &pos, tr->GetWorldScale().x, tr->GetWorldScale().y))			// Check if mouse is inside object
		{
			handler->ChangeScene();
		}
	}
	void Button::Shutdown()
	{
		RemoveFromSystem();
	}
	bool Button::Edit()
	{
		return false;
	}
	void Button::StreamRead(const nlohmann::json& j)
	{
	}
	void Button::StreamWrite(nlohmann::json& j) const
	{
	}
}