#include "Key.h"
#include "imgui/imgui.h"
#include "Engine/Graphics/Components/AEXGfxComps.h"
#include <iostream>
#include "Collisions/AEXCollisionSystem.h"

// REMOVE THIS AREBEK
#include "Platform/AEXInput.h"
namespace AEX
{
	void Key::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void Key::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void Key::OnCreate()
	{

	}
	void Key::Initialize()
	{
		mOwner->mEvents.subscribe<CollisionStayEvent, Key>(this, &Key::KeyGrab);
		player = aexScene->FindObjectByNameInAnySpace("Player")->GetComp<PlayerMovement>();
	}
	void Key::Update()
	{
	}
	void Key::Shutdown()
	{
		RemoveFromSystem();
	}
	bool Key::Edit()
	{
		ImGui::InputText("Name of the key", receiver_, 30);
		return true;
	}
	void Key::StreamRead(const nlohmann::json& j)
	{
		//std::string i = j["receiver_"];
		//receiver_ = i.c_str();
	}
	void Key::StreamWrite(nlohmann::json& j) const
	{
		j["receiver_"] = receiver_;
	}

	void Key::KeyGrab(const CollisionStayEvent& collision)
	{
		if (collision.Contact.obj[0]->GetOwner() == GetOwner() || collision.Contact.obj[1]->GetOwner() == GetOwner())
		{
			//debug AREBEK :: IDK IF THIS 'E' IS OK
			if (aexInput->KeyTriggered('E'))
			{
				keyname = receiver_;
				auto it = player->items.find(keyname);
				if (it == player->items.end())
				{
					player->items[keyname] = 1;
				}
				else
				{
					player->items[keyname] += 1;
				}	
			}
		}
	}

	void Key::PrintDebugState() 
	{
	}
}