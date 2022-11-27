#include "Door.h"
#include "iostream"


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
		mEvents.subscribe<PressurePlateEvent, Door>(this, &Door::DoorActivated);
	}
	void Door::Initialize()
	{
	}
	void Door::Update()
	{
	}
	void Door::Shutdown()
	{
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
	void Door::DoorActivated(const PressurePlateEvent& event)
	{
		// Check if this door is the one affected by the button
		if (strcmp(event.receiver_.c_str(),GetName()) != 0)	
			std::cout << "Abrete sesamo " << std::endl;
	}
}