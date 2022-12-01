#include "Door.h"
#include "iostream"
#include "Engine/Graphics/Components/AEXGfxComps.h"
#include "Collisions/AEXCollisionSystem.h"
#include "imgui/imgui.h"

namespace AEX
{
	void Door::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void Door::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void Door::OnCreate()
	{		
		mEvents.subscribe<CollisionStayEvent, Door>(this, &Door::DoorActivated);
	}
	void Door::Initialize()
	{
	}
	void Door::Update()
	{
	}
	void Door::Shutdown()
	{
		std::string evName = typeid(CollisionStayEvent).name();

		mEvents.unsubscribe(*mEvents.AllEvents[evName][0], evName);
		RemoveFromSystem();
	}
	bool Door::Edit()
	{
		return false;
	}
	void Door::StreamRead(const nlohmann::json& j)
	{
	}
	void Door::StreamWrite(nlohmann::json& j) const
	{
	}
	void Door::DoorActivated(const CollisionStayEvent& collision)
	{
		if (collision.Contact.obj[0]->GetOwner() == GetOwner() || collision.Contact.obj[1]->GetOwner() == GetOwner())
		{
			if (this->GetOwner()->GetComp<Collider>()->Ghost == false)
			{
				
			}
			else if (this->GetOwner()->GetComp<Collider>()->Ghost == true)
			{
				//change scenes
			}
		}
	}
}