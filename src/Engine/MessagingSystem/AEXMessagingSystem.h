#pragma once
#include "Event.h"

namespace AEX
{
	class HandlerFunction;
	class Event;
	struct EventContainer
	{
		template <typename EVT_TYPE>
		void subscribe(HandlerFunction* handler)
		{
			std::string eventName = typeid(EVT_TYPE).name();
			for (auto& it2 : AllEvents[eventName])      // Avoid duplicated listeners
				if (it2 == handler)
					return;

			AllEvents[eventName].push_back(handler);
		}
		template <typename EVT_TYPE, typename CLASS>
		void subscribe(CLASS* obj, void(CLASS::* func)(const EVT_TYPE& event))
		{
			std::string eventName = typeid(EVT_TYPE).name();
			auto handler = new MemberFunctionHandler<CLASS,EVT_TYPE>(obj, func);
			AllEvents[eventName].push_back(handler);
		}

		void unsubscribe(HandlerFunction& listener, std::string event);

		template <typename EVT_TYPE, typename CLASS>
		void unsubscribe(CLASS* obj, void(CLASS::* func)(const EVT_TYPE& event))
		{
			std::string eventName = typeid(EVT_TYPE).name();
			auto it = AllEvents.find(eventName);
			if ( it != AllEvents.end())
			{
				auto& vec = it->second;

				// Search for the matching listener and remove it
				for(auto vecIt = vec.begin(); vecIt != vec.end(); ++vecIt)
				{
					// get the handler pointer. 
					auto handlerPtr = *vecIt;

					// try to cast to a member function handler
					if (MemberFunctionHandler<CLASS, EVT_TYPE>* memberFn = 
						dynamic_cast<MemberFunctionHandler<CLASS, EVT_TYPE>*>(handlerPtr))
					{
						// is it the one we're trying to remove?
						if (memberFn->instance == obj && memberFn->function == func)
						{
							// Note(Iñaki?): Do we need to check for duplicate? Assuming no for now.
							vec.erase(vecIt);
							return;
						}
					}
				}
			}
		}

		void trigger_event(const Event& event);


	protected:
		std::map<std::string, std::vector<HandlerFunction*>> AllEvents;
	};
}