#include "Button.h"
#include "MessagingSystem/AEXMessagingSystem.h"
#include "imgui/imgui.h"
#include "Engine/Graphics/Components/AEXGfxComps.h"
#include <iostream>
namespace AEX
{
	void Button::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void Button::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void Button::OnCreate()
	{

	}
	void Button::Initialize()
	{
	}
	void Button::Update()
	{
	}
	void Button::Shutdown()
	{
		RemoveFromSystem();
	}
	bool Button::Edit()
	{
		if (ImGui::CollapsingHeader("Button"))
		{
			//ImGui::InputText("Put name of affected object", receiver_, 30);
			ImGui::InputText("Put buttonID", id_, 2);
		}
		return false;
	}
	void Button::StreamRead(const nlohmann::json& j)
	{
		//std::string i = j["receiver_"];
		//receiver_ = i.c_str();

		std::string i = j["id_"];
		id_[0] = i.c_str()[0];

	}
	void Button::StreamWrite(nlohmann::json& j) const
	{
		//j["receiver_"] = receiver_;
		j["id_"] = id_;

	}

	void Button::Pressed(const CollisionEnterEvent& collision)
	{
		// First check if the button is the one that has collided
		// Then send a message to the objects that are subscribed to this

		if (!pressed)
		{
			//TODO:: triger event for Door
			if (puzzle.OnButtonPressed(id_[0]))
			{
				Renderable* renderable = GetOwner()->GetComp<Renderable>();
				renderable->SetModColor(Color(0, 1, 0, 1));
			}
			pressed = true;
			
		}
		
			
	}
}

