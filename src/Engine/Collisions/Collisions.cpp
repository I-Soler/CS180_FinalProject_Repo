#include "Collisions.h"

namespace AEX
{
	bool StaticPointToStaticCircle(AEVec2* P, AEVec2* Center, float Radius)
	{
		f32 distance = P->Distance(*Center);

		if (distance <= Radius)
			return true;

		else
			return false;
	}
	bool StaticPointToStaticRect(AEVec2* Pos, AEVec2* Rect, float Width, float Height)
	{
		// Distance from center to point 
		AEVec2 lenght(abs(Pos->x - Rect->x), abs(Pos->y - Rect->y));

		// Detect if the point is between both sides of the square, horizontaly and vertically
		if (lenght.x * 2.0f <= Width && lenght.y * 2.0f <= Height)
			return true;
		else
			return false;
	}
	bool StaticPointToOrientedRect(AEVec2* Pos, AEVec2* Rect, float Width, float Height, float AngleRad)
	{
		// Rotate the rectangle and point
		AEMtx33 rotator = AEMtx33::RotRad(-AngleRad);
		AEVec2 rotPos = rotator.MultVec(*Pos);
		AEVec2 rotRect = rotator.MultVec(*Rect);

		// Call previous function with positions rotated
		if (StaticPointToStaticRect(&rotPos, &rotRect, Width, Height))
			return true;
		else
			return false;

		return StaticPointToStaticRect(Pos, Rect, Width, Height);
	}
	bool StaticCircleToStaticCircle(AEVec2* Center0, float Radius0, AEVec2* Center1, float Radius1)
	{
		return StaticPointToStaticCircle(Center0, Center1, Radius0 + Radius1);
	}
	bool StaticRectToStaticRect(AEVec2* Rect0, float Width0, float Height0, AEVec2* Rect1, float Width1, float Height1)
	{
		return StaticPointToStaticRect(Rect0, Rect1, Width0 + Width1, Height0 + Height1);
	}
	bool StaticRectToStaticCirlce(AEVec2* Rect, float Width, float Height, AEVec2* Center, float Radius)
	{
		float halfWidth = Width / 2;
		float halfHeight = Height / 2;

		// Get distance from center to point	
		AEVec2 lenghtRecCir(abs(Center->x - Rect->x), abs(Center->y - Rect->y));

		// Get all 4 edges
		AEVec2 RectPoint1(Rect->x + halfWidth, Rect->y + halfHeight); //Ar-Der
		AEVec2 RectPoint2(Rect->x - halfWidth, Rect->y + halfHeight); //Ar-Izq
		AEVec2 RectPoint3(Rect->x + halfWidth, Rect->y - halfHeight); //Ab-Der
		AEVec2 RectPoint4(Rect->x - halfWidth, Rect->y - halfHeight); //Ab-Izq


		// If the distance is smalles than the sum of both radiuses, it's inside
		if (lenghtRecCir.x <= Radius + halfWidth && lenghtRecCir.y <= Radius + halfHeight)
		{
			// Check for collision in the edges
			if ((lenghtRecCir.x >= halfWidth && lenghtRecCir.y >= halfHeight) && StaticPointToStaticCircle(Center, &RectPoint1, Radius))
				return true;
			else if ((lenghtRecCir.x >= halfWidth && lenghtRecCir.y >= halfHeight) && StaticPointToStaticCircle(Center, &RectPoint2, Radius))
				return true;
			else if ((lenghtRecCir.x >= halfWidth && lenghtRecCir.y >= halfHeight) && StaticPointToStaticCircle(Center, &RectPoint3, Radius))
				return true;
			else if ((lenghtRecCir.x >= halfWidth && lenghtRecCir.y >= halfHeight) && StaticPointToStaticCircle(Center, &RectPoint4, Radius))
				return true;
		}

		else
			return false;
	}
	bool OrientedRectToStaticCirlce(AEVec2* Rect, float Width, float Height, float AngleRad, AEVec2* Center, float Radius)
	{
		// Rotate the rectangle and point
		AEMtx33 rotator = AEMtx33::RotRad(-AngleRad);
		AEVec2 rotRect = rotator.MultVec(*Rect);
		AEVec2 rotCir = rotator.MultVec(*Center);

		//Call previous fuction but with rotated vectors
		if (StaticRectToStaticCirlce(&rotRect, Width, Height, &rotCir, Radius))
			return true;
		else
			return false;
	}
}
