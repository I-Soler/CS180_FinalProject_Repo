#include <AEX.h>
#include "AEXMessagingSystem.h"

namespace AEX
{
	//bool MessagingSystem::Initialize()
	//{
	//	return true;
	//}
	///*void MessagingSystem::Update()
	//{
	//	auto input = aexInput;

	//	if (input->KeyPressed('W'))
	//		trigger_event(WASDInput(WASDInput::W));
	//	if (input->KeyPressed('A'))
	//		trigger_event(WASDInput(WASDInput::A));
	//	if (input->KeyPressed('S'))
	//		trigger_event(WASDInput(WASDInput::S));
	//	if (input->KeyPressed('D'))
	//		trigger_event(WASDInput(WASDInput::D));

	//	if (input->GetMouseWheel() > 0)
	//		trigger_event(WASDInput(WASDInput::MouseUp));
	//	if (input->GetMouseWheel() < 0)
	//		trigger_event(WASDInput(WASDInput::MouseDown));

	//	
	//}*/
	void EventContainer::unsubscribe(HandlerFunction& listener, std::string event)
	{
		// Search for the matching listener and remove it
		for ( int it = 0; it < AllEvents[event].size(); it++)
		{
			if (AllEvents[event][it] == &listener)
				AllEvents[event].erase((AllEvents[event].begin() + it));
		}
	}
	void EventContainer::trigger_event(const Event& event)
	{
		if (AllEvents.find(typeid(event).name()) == AllEvents.end())
			return;

		auto& handlers = AllEvents[typeid(event).name()];
		for (auto& it : handlers)      // call every function that is asociated to that event
		{
			it->handle(event);
		}
	}
}