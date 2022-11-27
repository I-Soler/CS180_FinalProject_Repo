#pragma once
#include "aexmath/AEXMath.h"

namespace AEX
{
	bool StaticPointToStaticCircle(AEVec2* P, AEVec2* Center, float Radius);
	bool StaticPointToStaticRect(AEVec2* Pos, AEVec2* Rect, float Width, float Height);
	bool StaticPointToOrientedRect(AEVec2* Pos, AEVec2* Rect, float Width, float Height, float AngleRad);

	bool StaticCircleToStaticCircle(AEVec2* Center0, float Radius0, AEVec2* Center1, float Radius1);
	bool StaticRectToStaticRect(AEVec2* Rect0, float Width0, float Height0, AEVec2* Rect1, float Width1, float Height1);
	bool StaticRectToStaticCirlce(AEVec2* Rect, float Width, float Height, AEVec2* Center, float Radius);
	bool OrientedRectToStaticCirlce(AEVec2* Rect, float Width, float Height, float AngleRad, AEVec2* Center, float Radius);
}