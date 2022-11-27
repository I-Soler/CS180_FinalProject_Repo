//-----------------------------------------------------------------------------
#ifndef AEX_TRANSFORM_H_
#define AEX_TRANSFORM_H_
//-----------------------------------------------------------------------------
#include "AEXMathDLL.h"

namespace AEX
{
	struct AEX_MATH_API Transform
	{
		Transform();
		Transform(const AEVec3 & pos, const AEVec2 & scale, f32 rot);

		AEVec2	mScale;				// X-Y Scale
		AEVec3	mTranslation;		// Position (x,y,z) for rendering ignored for 3x3 matrix
		f32	mOrientation;			// Angle of Rotation

		// wrapper functions around globals;
		AEMtx33 GetMatrix();
		AEMtx33 GetInvMatrix();
		AEMtx44 GetMatrix44();
		AEMtx44 GetInvMatrix44();

		Transform Concat(const Transform &rhs);
		Transform InverseConcat(const Transform& rhs)const;
		AEVec2		MultVec(const AEVec2 &rhs);
		AEVec2		InvMultVec(const AEVec2& rhs);
		AEVec3		InvMultVec(const AEVec3& rhs);
		AEVec2		operator *(const AEVec2 & rhs);
		Transform	operator *(const Transform &rhs);
		Transform&	operator *=(const Transform& rhs);
	};
}
#endif // TRANSFORM_H_