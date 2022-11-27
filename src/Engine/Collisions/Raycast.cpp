#include <aexmath/AEXMath.h>
#include "LineSegment2D.h"
#include "Raycast.h"
#define BIGTIME 100000
namespace AEX
{
	
	//! Performs a raycast test against a line.
	f32 RayCastLine(const AEVec2& origin, const AEVec2& dir, const LineSegment2D& line, AEVec2* outPi)
	{
		AEVec2 mPoint;

		// set time of intersection
		float time = ((line.mP0 * line.mN) - (origin * line.mN)) / (dir * line.mN);

		mPoint = origin + dir * time;

		AEVec2 LineVec1 = line.mP1 - line.mP0;
		AEVec2 LineVec2 = line.mP0 - line.mP1;
		AEVec2 LinePointVec1 = mPoint - line.mP0;
		AEVec2 LinePointVec2 = mPoint - line.mP1;

		if (LineVec1 * LinePointVec1 < 0 || LineVec2 * LinePointVec2 < 0)   // Check fake positives
			return -1;

		// Set intersection point
		*outPi = mPoint;
		return time;
	}

	
	//! Performs a raycast test against an obb.
	f32 RayCastRect(const AEVec2& origin, const AEVec2& dir, const Transform& rect, AEVec2* outPi)
	{
		AEMtx33 rotator = AEMtx33::RotRad(rect.mOrientation);
		AEVec2 mScaleHalf(rect.mScale.x / 2, rect.mScale.y / 2);

		// Get vertexes
		AEVec2 vertex1(mScaleHalf.x, mScaleHalf.y);  // Up right
		vertex1 = rotator.MultVec(vertex1);
		vertex1 += AEVec2(rect.mTranslation.x, rect.mTranslation.y);

		AEVec2 vertex2(-mScaleHalf.x, mScaleHalf.y);  // Up left
		vertex2 = rotator.MultVec(vertex2);
		vertex2 += AEVec2(rect.mTranslation.x, rect.mTranslation.y);

		AEVec2 vertex3(-mScaleHalf.x, -mScaleHalf.y);  // Down left
		vertex3 = rotator.MultVec(vertex3);
		vertex3 += AEVec2(rect.mTranslation.x, rect.mTranslation.y);

		AEVec2 vertex4(mScaleHalf.x, -mScaleHalf.y);  // Down right
		vertex4 = rotator.MultVec(vertex4);
		vertex4 += AEVec2 (rect.mTranslation.x, rect.mTranslation.y);

		// Get lines of the obb
		LineSegment2D rect1(vertex1, vertex2);	 // Up
		LineSegment2D rect2(vertex2, vertex3);	 // Left
		LineSegment2D rect3(vertex3, vertex4);	 // Down
		LineSegment2D rect4(vertex4, vertex1);	 // Right

		// Get time values
		float time4 = RayCastLine(origin, dir, rect4, outPi);
		float time3 = RayCastLine(origin, dir, rect3, outPi);
		float time2 = RayCastLine(origin, dir, rect2, outPi);
		float time1 = RayCastLine(origin, dir, rect1, outPi);

		// get smallest time and update the point of intersection to match the time

		float time = BIGTIME;  // big number to avoid errors

		if (time1 < time && time1 > EPSILON)
			time = RayCastLine(origin, dir, rect1, outPi);

		if (time2 < time && time2 > EPSILON)
			time = RayCastLine(origin, dir, rect2, outPi);

		if (time3 < time && time3 > EPSILON)
			time = RayCastLine(origin, dir, rect3, outPi);

		if (time4 < time && time4 > EPSILON)
			time = RayCastLine(origin, dir, rect4, outPi);

		if (time == BIGTIME)
			return -1;

		return time;
	}

	
	//! Performs a raycast test against a circle.
	f32 RayCastCircle(const AEVec2& origin, const AEVec2& dir, const AEVec2& circle, f32 radius, AEVec2* outPi)
	{
		float time = 0;
		float time1 = 0;
		float time2 = 0;

		// Quadratic formula variables
		float A = dir * dir;
		float B = -2 * ((circle - origin) * dir);
		float C = (circle - origin) * (circle - origin) - (radius * radius);
		float Delta = B * B - 4 * A * C;

		if (Delta < EPSILON)  // ray misses circle
			return -1;
		else if (Delta > EPSILON)
		{
			// quadratic formula
			time1 = ((-B + sqrt(Delta)) / 2 * Delta);
			time2 = ((-B - sqrt(Delta)) / 2 * Delta);

			if (time1 > 0 && time2 > 0)  // Intersect with both sides of the circle, get closest one
			{
				if (time1 < time2)
					time = time1;
				else
					time = time2;
			}

			if (time1 < 0 && time2 < 0)  // fake positive check
				return -1;

			if ((time1 < 0 && time2 > 0) || (time1 > 0 && time2 < 0)) // point is inside circle
			{
				if (time1 > time2)
					time = time1;
				else
					time = time2;
			}

		}
		else
			time = -B / 2 * A;

		if (time > 0 && time < 1)   // false positive check
			return -1;

		// set intersection point
		*outPi = origin + dir * time;

		return time;
	}



	//! Performs a raycast test against a line. This function should call the RayCastLine function above.
	f32 RayCastLine(const Ray& ray, const LineSegment2D& line, AEVec2* outPi)
	{
		return RayCastLine(ray.mOrigin, ray.mDirection, line, outPi);
	}

	
	//! Performs a raycast test against an obb. This function should call the RayCastRect function above.
	f32 RayCastRect(const Ray& ray, const Transform& rect, AEVec2* outPi)
	{
		return RayCastRect(ray.mOrigin, ray.mDirection, rect, outPi);
	}

	
	//! Performs a raycast test against a circle. This function should call the RayCastCircle function above.
	f32 RayCastCircle(const Ray& ray, const AEVec2& circle, f32 radius, AEVec2* outPi)
	{
		return RayCastCircle(ray.mOrigin, ray.mDirection, circle, radius, outPi);
	}
}
