#include "InfoScreen.h"
#include "imgui/imgui.h"
#include "Engine/Graphics/Components/AEXGfxComps.h"
#include <iostream>
#include "Collisions/AEXCollisionSystem.h"

// REMOVE THIS AREBEK
#include "Platform/AEXInput.h"
namespace AEX
{
	void InfoScreen::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void InfoScreen::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void InfoScreen::OnCreate()
	{

	}
	void InfoScreen::Initialize()
	{
		scaley = this->GetOwner()->GetComp<TransformComp>()->GetScale().y;
	}
	void InfoScreen::Update()
	{
		if (hidden)
			return;
	}
	void InfoScreen::Shutdown()
	{
		RemoveFromSystem();
	}
	bool InfoScreen::Edit()
	{
		return true;
	}
	void InfoScreen::StreamRead(const nlohmann::json& j)
	{
	}
	void InfoScreen::StreamWrite(nlohmann::json& j) const
	{
	}
	void InfoScreen::Show()
	{
		this->GetOwner()->GetComp<TransformComp>()->SetScale(AEVec2(0.0f, 0.0f));
		hidden = false;
	}

	void InfoScreen::Hide()
	{
		this->GetOwner()->GetComp<TransformComp>()->SetScale(AEVec2(0.0f,0.0f));
		hidden = true;
	}


	void InfoScreen::PrintDebugState() 
	{
		std::cout << "  STATE : " << hidden << std::endl;
	}
}