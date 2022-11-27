#include "PressurePlate.h"
#include "imgui/imgui.h"
#include "Engine/Graphics/Components/AEXGfxComps.h"
#include <iostream>
#include "Collisions/AEXCollisionSystem.h"

// REMOVE THIS AREBEK
#include "Platform/AEXInput.h"
namespace AEX
{
	void PressurePlate::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void PressurePlate::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void PressurePlate::OnCreate()
	{

	}
	void PressurePlate::Initialize()
	{
		mOwner->mEvents.subscribe<CollisionStayEvent, PressurePlate>(this, &PressurePlate::Pressed);

		ResetPressurePlate();

		pressPuzzle = aexScene->FindObjectByNameInAnySpace("presspuzzle")->GetComp<PressPuzzleController>();
		pressPuzzle->AddPressurePlate(this);
	}
	void PressurePlate::Update()
	{
	}
	void PressurePlate::Shutdown()
	{
		RemoveFromSystem();
	}
	bool PressurePlate::Edit()
	{
		if (ImGui::CollapsingHeader("PressurePlate"))
		{
			//ImGui::InputText("Put name of affected object", receiver_, 30);
			ImGui::InputText("Put PressurePlateID", id_, 2);
		}
		return false;
	}
	void PressurePlate::StreamRead(const nlohmann::json& j)
	{
		//std::string i = j["receiver_"];
		//receiver_ = i.c_str();

		std::string i = j["id_"];
		id_[0] = i.c_str()[0];

	}
	void PressurePlate::StreamWrite(nlohmann::json& j) const
	{
		//j["receiver_"] = receiver_;
		j["id_"] = id_;

	}

	void PressurePlate::Pressed(const CollisionStayEvent& collision)
	{
		// First check if the PressurePlate is the one that has collided
		// Then send a message to the objects that are subscribed to this
		if (!pressed)
		{
			if (collision.Contact.obj[0]->GetOwner() == GetOwner() || collision.Contact.obj[1]->GetOwner() == GetOwner())
			{
				//AREBEK REMOVE IF STATEMENT
				if(aexInput->KeyTriggered(Keys::Space))
				{
					//std::cout << "AREBEK PressurePlate PRESSED : " << id_[0] << std::endl;
					mEvents.trigger_event(PressurePlateEvent(std::string("a")));
					if (pressPuzzle->OnPressurePlatePressed(id_[0]))
					{
						Renderable* renderable = GetOwner()->GetComp<Renderable>();
						renderable->SetModColor(Color(0, 1, 0, 1));
						pressed = true;
					}
				}
			}
		}


	}

	void PressurePlate::ResetPressurePlate()
	{
		pressed = false;
		Renderable* renderable = GetOwner()->GetComp<Renderable>();
		renderable->SetModColor(Color(1, 1, 1, 1));
	}

	void PressurePlate::SetEndState()
	{
		Renderable* renderable = GetOwner()->GetComp<Renderable>();
		renderable->SetModColor(Color(1, 0, 0, 1));
	}

	void PressurePlate::PrintDebugState() {
		std::cout << "PressurePlate : " << id_[0];
		std::cout << "  STATE : " << pressed << std::endl;
	}

}
