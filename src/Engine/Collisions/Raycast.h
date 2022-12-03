#ifndef RAYCAST_H_
#define RAYCAST_H_
#include <Collisions/LineSegment2D.h>


namespace AEX
{
	/*!
	\struct Ray
	\brief	 Represents a 2-dimensional ray with an origina and direction.
	\remark Direction is not assumed to be normalized.
	*/
	struct Ray
	{
		AEVec2 mOrigin;
		AEVec2 mDirection;
	};

	
	//! Performs a raycast test against a line.
	f32 RayCastLine(const AEVec2& origin, const AEVec2& dir, const LineSegment2D& line, AEVec2* outPi);
	
	//! Performs a raycast test against an obb.
	f32 RayCastRect(const AEVec2& origin, const AEVec2& dir, const Transform& rect, AEVec2* outPi);
	
	//! Performs a raycast test against a circle.
	f32 RayCastCircle(const AEVec2& origin, const AEVec2& dir, const AEVec2& circle, f32 radius, AEVec2* outPi);

	
	//! Performs a raycast test against a line. This function should call the RayCastLine function above.
	f32 RayCastLine(const Ray& ray, const LineSegment2D& line, AEVec2* outPi);
	
	//! Performs a raycast test against an obb. This function should call the RayCastRect function above.
	f32 RayCastRect(const Ray& ray, const Transform& rect, AEVec2* outPi);
	
	//! Performs a raycast test against a circle. This function should call the RayCastCircle function above.
	f32 RayCastCircle(const Ray& ray, const AEVec2& circle, f32 radius, AEVec2* outPi);

}

#endif