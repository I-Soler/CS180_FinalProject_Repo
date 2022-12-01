#pragma once

#include "IEvent.h"

namespace AEX
{

	struct WASDInput : public Event
	{
		enum WASD {W,A,S,D, MouseUp, MouseDown};
		WASDInput(WASD type) : type_(type) {}

		WASD type_;
	};

}

