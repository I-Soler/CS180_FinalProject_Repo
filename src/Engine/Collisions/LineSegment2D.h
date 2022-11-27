// ---------------------------------------------------------------------------
#ifndef LINE_SEGMENT_2D_H
#define LINE_SEGMENT_2D_H
// ---------------------------------------------------------------------------
#include "aexmath/AEXMath.h"

namespace AEX
{
	struct LineSegment2D
	{
		AEVec2 mP0;		// Point on the line
		AEVec2 mP1;		// Point on the line
		AEVec2 mN;			// Line's normal
		float mNdotP0;		// To avoid computing it every time it's needed

		LineSegment2D();
		LineSegment2D(const AEVec2& p0, const AEVec2& p1);
	};

	/*!
	\brief	This function evaluates the point P in the implicit line equation
			as described in class( N dot P - N dot V0)

	\return	The result of the evaluation. Note that the returned value should be:
	- Negative if the point is in the line's inside half plane
	- Positive if the point is in the line's outside half plane
	- Zero if the point is on the line
	*/
	float StaticPointToStaticLineSegment(AEVec2* P, LineSegment2D* LS);

	/*!
	\brief	Given a point P and an array of LineSegment2D, determines if a point
	is contained by all line segments (ie. inside half plane of all segments).

	\details
		This function is equivalent to checking if a point is contained within
		the polygon defined by the line segments.

		Note: There are no checks to make sure that the line segment array
		forms a closed loop. This is up to the client API.

	\return	true if the point is inside all line segments, false otherwise.
	*/
	bool PointInLineSegments(AEVec2* P, LineSegment2D* LS, u32 count);


	// ---------------------------------------------------------------------------
}

#endif