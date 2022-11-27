#ifndef CONTACT_COLLISION_H
#define CONTACT_COLLISION_H

#include "aexmath/AEXMath.h"

namespace AEX
{
	struct Contact
	{
		AEVec2	mPi;
		AEVec2	mNormal;
		float	mPenetration;
	};

	bool StaticPointToStaticCircleEx(AEVec2* Pos, AEVec2* Center, float Radius, Contact* pResult);
	bool StaticPointToStaticAABBEx(AEVec2* Pos, AEVec2* rectPos, float rectWidth, float rectHeight, Contact* pResult);
	bool StaticPointToStaticOBBEx(AEVec2* Pos, Transform* OBB, Contact* pResult);

	bool StaticCircleToStaticCircleEx(AEVec2* Center1, float Radius1, AEVec2* Center2, float Radius2, Contact* pResult);


	bool StaticRectToStaticCircleEx(AEVec2* rectPos, float rectWidth, float rectHeight, AEVec2* circlePos, float radius, Contact* pResult);


	bool StaticOBBToStaticCircleEx(Transform* OBB, AEVec2* Center, float Radius, Contact* pResult);


	bool StaticRectToStaticRectEx(AEVec2* pos1, AEVec2* size1, AEVec2* pos2, AEVec2* size2, Contact* pResult);


	bool OrientedRectToOrientedRectEx(const Transform* OBB1, const Transform* OBB2, Contact* pResult);


}


#endif
