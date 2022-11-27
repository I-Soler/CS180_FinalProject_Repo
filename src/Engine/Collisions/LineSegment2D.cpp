#include "LineSegment2D.h"


namespace AEX
{
	LineSegment2D::LineSegment2D()
	{
		mP0.x = 0, mP0.y = 0;
		mP1.x = 0, mP1.y = 0;
		mN.x = 0, mN.y = 0;
		mNdotP0 = 0;
	}

	LineSegment2D::LineSegment2D(const AEVec2& p0, const AEVec2& p1)
	{
		mP0 = p0;
		mP1 = p1;

		AEVec2 Edge = p0 - p1;
		mN = Edge.Perp().Normalize();

		mNdotP0 = mN.Dot(p0);
	}

	float StaticPointToStaticLineSegment(AEVec2* P, LineSegment2D* LS)
	{

		if ((*P * LS->mN) - (LS->mP0 * LS->mN) < 0)		 // point is in the line's inside half plane	
			return -1;

		else if ((*P * LS->mN) - (LS->mP0 * LS->mN) > 0)  //  point is in the line's outside half plane	
			return 1;

		else											 // point is on the line
			return 0;
	}

	bool PointInLineSegments(AEVec2* P, LineSegment2D* LS, u32 count)
	{
		for (int it = 0; it < count; it++)
		{
			if (StaticPointToStaticLineSegment(P, &LS[it]) == 1)
				return false;
		}

		return true;
	}
}
