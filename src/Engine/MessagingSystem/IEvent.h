#pragma once
#include "Core/AEXCore.h"


namespace AEX
{
    class IBase;
	class Event
	{
	public:
		Event();

        void set_sender(IBase* _sender);
		virtual ~Event() {};

	private:
		//std::string mID;    // String ID
		bool mHandled;      // If true, don't propagate the message
		IBase* sender;      // Object that sent the event
	};

	class HandlerFunction
	{
	public:
		void handle(const Event& event);
		virtual void call(const Event& event) = 0;
		virtual ~HandlerFunction() {};
	};

    class VoidGlobalFunctionHandler : public HandlerFunction
    {
        void (*handler_func)();
        void call(const Event& event) {
            handler_func();
        }
    };

    class GlobalFunctionHandler : public HandlerFunction
    {
        void (*handler_func)(const Event & e);
        void call(const Event& event) {
            handler_func(event);
        }
    };

    template <typename T1, typename T2>
    class MemberFunctionHandler : public HandlerFunction
    {
        friend struct EventContainer;
    public:
        typedef void(T1::* MemberFunction)(const T2& event);
        MemberFunctionHandler(T1* object, MemberFunction fn)
        {
            instance = object;
            function = fn;
        }

        /**
         * @brief Calls a specific class sending an event
         *
         * @param event to be sent to the function
         */
        virtual void call(const Event& event)
        {
            (instance->*function)(static_cast<const T2&>(event));
        }

        ~MemberFunctionHandler() {}
    private:
        T1* instance;
        MemberFunction  function;

    };

    class EventHandler
    {
    public:
        template<typename T1, typename T2>
        /**
         * @brief Templated function that stores functions
         *        of an user defined class
         * @param app       class that has the function for the event
         * @param Function  pointer to function that does the event
         */
        void register_handler(T1& app, void(T1::* Function)(const T2& event))
        {
            // avoid duplicates
            std::string t(typeid(T2).name());
            delete MemberMap[t];

            MemberFunctionHandler<T1, T2>* member = new MemberFunctionHandler<T1, T2>(&app, Function);
            MemberMap[t] = member;
        }

        ~EventHandler();

        void handle(const Event& event);
    private:
        std::map<std::string, HandlerFunction*> MemberMap;
    };
}
