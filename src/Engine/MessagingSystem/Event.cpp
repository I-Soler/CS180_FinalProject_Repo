#include "Event.h"

namespace AEX
{
    Event::Event()
    {
        //mID = id;
        mHandled = false;
        sender = nullptr;
    }
    void HandlerFunction::handle(const Event& event)
    {
        call(event);
    }

    EventHandler::~EventHandler()
    {
        for (auto& it : MemberMap)
        {
            delete it.second;
        }
    }

    void EventHandler::handle(const Event& event)
    {
        MemberMap[typeid(event).name()]->call(event);
    }
}
