#include "IEvent.h"

namespace AEX
{
    Event::Event()
    {
        //mID = id;
        mHandled = false;
        sender = nullptr;
    }
    void Event::set_sender(IBase* _sender)
    {
        sender = _sender;
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
