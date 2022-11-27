#ifndef AEX_COLLISIONS_H_
#define AEX_COLLISIONS_H_

#include "AEXMath.h"

namespace AEX
{
	struct AEX_MATH_API Rect2D
	{
		AEX::AEVec2 p;
		AEX::AEVec2 size;
		bool Contains(const AEVec2 &pt);
	};
	struct AEX_MATH_API Line2D
	{
		AEVec2 start, end;
		inline AEVec2 GetNormal() { return (end - start).Perp().Normalize(); }
	};
	struct AEX_MATH_API Ray3D 
	{
		AEVec3 mOrigin;
		AEVec3 mDirection;
	};

	// ------------------------------------------------------------------------
	// Intersection FUNCTIONS
	// ------------------------------------------------------------------------

	// PointInRect
	bool AEX_MATH_API PointInRect(const AEVec2 &pt, const AEVec2 rect_pos, AEVec2 rect_size);

	// ------------------------------------------------------------------------
	// TestRecttoRect
	bool AEX_MATH_API TestRecttoRect(const Rect2D&a, const Rect2D&b);

	// ------------------------------------------------------------------------
	// TestPointToLine
	f32 AEX_MATH_API TestPointToLine(AEVec2 point, AEVec2 start_line, AEVec2 end_line);

	// ------------------------------------------------------------------------
	// TestRecttoLine
	bool AEX_MATH_API TestRecttoLine(Rect2D box, Line2D line);

	// ------------------------------------------------------------------------
	// RayToZPlane
	f32 AEX_MATH_API RayToZPlane(const Ray3D& ray, f32 z = 0.0f);
	f32 AEX_MATH_API RayToZPlane(const AEVec3 &r, const AEVec3 &d, f32 z = 0.0f);

	// ------------------------------------------------------------------------
	// RayToAABB
	f32 AEX_MATH_API RayToAABB(const Ray3D& ray, const AEVec3& minP, const AEVec3& maxP);
	f32 AEX_MATH_API RayToAABB(const AEVec3 &r, const AEVec3 &d, const AEVec3 &minP, const AEVec3 &maxP);

	// ------------------------------------------------------------------------
	// RayToOBB
	f32 AEX_MATH_API RayToOBB(const Ray3D& ray, const Transform3D& obb);
	f32 AEX_MATH_API RayToOBB(const AEVec3 &r, const AEVec3 &d, const Transform3D &obb);

} // namespace AEX

// ----------------------------------------------------------------------------
#endif