#include "ContactCollisions.h"
#include "Collisions.h"

namespace AEX
{
	AEVec2 CheckVertex(AEVec2* pos1, AEVec2* size1, AEVec2* pos2, AEVec2* size2);


	bool StaticPointToStaticCircleEx(AEVec2* Pos, AEVec2* Center, float Radius, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		if (StaticPointToStaticCircle(Pos, Center, Radius))
		{
			pResult->mPi = *Pos;
			pResult->mPenetration = abs(Pos->Distance(*Center));
			pResult->mNormal = *Pos - *Center;
			return true;
		}
		else
			return false;
	}

	bool StaticPointToStaticAABBEx(AEVec2* Pos, AEVec2* rectPos, float rectWidth, float rectHeight, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		if (StaticPointToStaticRect(Pos, rectPos, rectWidth, rectHeight))
		{
			pResult->mPi = *Pos;
			pResult->mPenetration = abs(Pos->Distance(*rectPos));
			pResult->mNormal = *Pos - *rectPos;
			return true;
		}
		else
			return false;
	}

	bool StaticPointToStaticOBBEx(AEVec2* Pos, Transform* OBB, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		AEVec2 OBB2D(OBB->mTranslation.x, OBB->mTranslation.y);

		if (StaticPointToOrientedRect(Pos, &OBB2D, OBB->mScale.x, OBB->mScale.y, OBB->mOrientation))
		{
			pResult->mPi = *Pos;
			pResult->mPenetration = abs(Pos->Distance(OBB2D));
			pResult->mNormal = *Pos - OBB2D;
			return true;
		}
		else
			return false;
	}



	bool StaticCircleToStaticCircleEx(AEVec2* Center1, float Radius1, AEVec2* Center2, float Radius2, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		if (StaticCircleToStaticCircle(Center1, Radius1, Center2, Radius2))   //If collision detected
		{
			//Store variables
			pResult->mNormal.x = (Center2->x - Center1->x) / Center2->Distance(*Center1);
			pResult->mNormal.y = (Center2->y - Center1->y) / Center2->Distance(*Center1);

			pResult->mPenetration = Radius1 + Radius2 - Center2->Distance(*Center1);

			pResult->mPi.x = Center1->x + pResult->mNormal.x * Radius1;
			pResult->mPi.y = Center1->y + pResult->mNormal.y * Radius1;

			pResult->mNormal = -pResult->mNormal;

			return true;
		}
		else
			return false;
	}


	bool StaticRectToStaticCircleEx(AEVec2* rectPos, float rectWidth, float rectHeight, AEVec2* circlePos, float radius, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		if (StaticRectToStaticCirlce(rectPos, rectWidth, rectHeight, circlePos, radius))
		{
			if (StaticPointToStaticRect(circlePos, rectPos, rectWidth, rectHeight)) // If the circle is entirely inside the rectangle
			{
				AEVec2 dist;
				dist.x = circlePos->x - rectPos->x;
				dist.y = circlePos->y - rectPos->y;

				//calculate penetration
				AEVec2 penetration;
				penetration.x = rectWidth / 2 - abs(dist.x);
				penetration.y = rectHeight / 2 - abs(dist.y);

				//Store variables
				if (penetration.x < penetration.y)   //Check if penetration goes upwards or downwards
				{
					pResult->mNormal.x = 1;
					pResult->mNormal.y = 0;
				}
				else
				{
					pResult->mNormal.x = 0;
					pResult->mNormal.y = 1;
				}
				if (dist * pResult->mNormal < 0) //if the normal points in the other direction
					pResult->mNormal *= -1;

				pResult->mPenetration = Min(penetration.x, penetration.y) + radius;

				pResult->mPi.x = circlePos->x;
				pResult->mPi.y = circlePos->y;
			}
			else   // Circle center is outside
			{
				AEVec2 clamp;
				clamp.x = Clamp(circlePos->x, rectPos->x - rectWidth / 2, rectPos->x + rectWidth / 2);
				clamp.y = Clamp(circlePos->y, rectPos->y - rectHeight / 2, rectPos->y + rectHeight / 2);

				//Store variables
				pResult->mNormal.x = (circlePos->x - clamp.x) / circlePos->Distance(clamp);
				pResult->mNormal.y = (circlePos->y - clamp.y) / circlePos->Distance(clamp);

				pResult->mPenetration = radius - circlePos->Distance(clamp);

				pResult->mPi.x = clamp.x;
				pResult->mPi.y = clamp.y;
			}

			return true;
		}
		else
			return false;

	}


	bool StaticOBBToStaticCircleEx(Transform* OBB, AEVec2* Center, float Radius, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		// rotate cicle and rectangle
		AEMtx33 rotator = AEMtx33::RotRad(-OBB->mOrientation);
		AEMtx33 InvRotator = AEMtx33::RotRad(OBB->mOrientation);
		AEVec2 rotRect = rotator.MultVec(OBB->mTranslation);
		AEVec2 rotCir = rotator.MultVec(*Center);

		if (StaticRectToStaticCircleEx(&rotRect, OBB->mScale.x, OBB->mScale.y, &rotCir, Radius, pResult))  // detect collision
		{
			// de-rotate the normal and Pi
			pResult->mNormal = InvRotator.MultVec(pResult->mNormal);
			pResult->mPi = InvRotator.MultVec(pResult->mPi);
			return true;
		}
		else
			return false;
	}


	bool StaticRectToStaticRectEx(AEVec2* pos1, AEVec2* size1, AEVec2* pos2, AEVec2* size2, Contact* pResult)
	{
		if (pResult == NULL)
			return false;

		if (StaticRectToStaticRect(pos1, size1->x, size1->y, pos2, size2->x, size2->y))  // If collision is detected
		{

			AEVec2 dist;
			dist.x = pos2->x - pos1->x;
			dist.y = pos2->y - pos1->y;

			//calculate penetration
			AEVec2 penetration;
			penetration.x = (size1->x + size2->x) / 2 - abs(dist.x);
			penetration.y = (size1->y + size2->y) / 2 - abs(dist.y);

			//Store variables
			if (penetration.x < penetration.y)   //Check if penetration goes upwards or downwards
			{
				pResult->mPenetration = penetration.x;
				pResult->mNormal.x = 1;
				pResult->mNormal.y = 0;
			}
			else
			{
				pResult->mPenetration = penetration.y;
				pResult->mNormal.x = 0;
				pResult->mNormal.y = 1;
			}
			if (dist * pResult->mNormal < 0) //if the normal points in the other direction
				pResult->mNormal *= -1;

			AEVec2 vertex;

			vertex = CheckVertex(pos1, size1, pos2, size2);
			pResult->mPi.x = vertex.x;
			pResult->mPi.y = vertex.y;

			return true;
		}
		else
			return false;
	}


	bool OrientedRectToOrientedRectEx(const Transform* OBB1, const Transform* OBB2, Contact* pResult)
	{
		// Half sides of the rectangles
		float half_W1 = OBB1->mScale.x / 2;
		float half_H1 = OBB1->mScale.y / 2;
		float half_W2 = OBB2->mScale.x / 2;
		float half_H2 = OBB2->mScale.y / 2;

		// Rotation matrixes
		AEMtx33 rotMtx1 = AEMtx33::RotRad(OBB1->mOrientation);	// Rotation of OBB1
		AEMtx33 rotMtx2 = AEMtx33::RotRad(OBB2->mOrientation);	// Rotation of OBB2

		// Vertexes of OBB1
		AEVec2 corners1[4];
		corners1[0] = OBB1->mTranslation + rotMtx1.MultVec(AEVec2(-half_W1, half_H1));
		corners1[1] = OBB1->mTranslation + rotMtx1.MultVec(AEVec2(half_W1, half_H1));
		corners1[2] = OBB1->mTranslation + rotMtx1.MultVec(AEVec2(half_W1, -half_H1));
		corners1[3] = OBB1->mTranslation + rotMtx1.MultVec(AEVec2(-half_W1, -half_H1));

		// Vertexes of OBB2
		AEVec2 corners2[4];
		corners2[0] = OBB2->mTranslation + rotMtx2.MultVec(AEVec2(-half_W2, half_H2));
		corners2[1] = OBB2->mTranslation + rotMtx2.MultVec(AEVec2(half_W2, half_H2));
		corners2[2] = OBB2->mTranslation + rotMtx2.MultVec(AEVec2(half_W2, -half_H2));
		corners2[3] = OBB2->mTranslation + rotMtx2.MultVec(AEVec2(-half_W2, -half_H2));


		AEVec2 eachProj, maxProj, minProj, sepVec1, sepVec2, totalProj, normal;	// Holders
		float minDot, maxDot, minPenet = INFINITY;	// Holder for dot products

		//AEVec2 normals1[2] = { rotMtx1.MultVec(AEVec2(1, 0)), rotMtx1.MultVec(AEVec2(0, 1)) };	// Normals of p1
		//AEVec2 normals2[2] = { rotMtx2.MultVec(AEVec2(1, 0)), rotMtx2.MultVec(AEVec2(0, 1)) };	// Normals of p2

		AEVec2 normals1[4] = 
		{ rotMtx1.MultVec(AEVec2(1, 0)), rotMtx1.MultVec(AEVec2(0, 1)) ,	// Normals of p1
		  rotMtx2.MultVec(AEVec2(1, 0)), rotMtx2.MultVec(AEVec2(0, 1)) };	// Normals of p2


		// For each normal of p1, project all vertices of p1 and p2
		for (int n1 = 0; n1 < 4; n1++)
		{
			// Project all vertices of p2 into normal of p1
			for (int v2 = 0; v2 < 4; v2++)
			{
				eachProj = corners2[v2].Project(normals1[n1]);
				if (!v2)	// Take first projection as min and max
				{
					minProj = eachProj;
					maxProj = eachProj;
					minDot = minProj.Dot(normals1[n1]);
					maxDot = maxProj.Dot(normals1[n1]);
				}
				else
				{
					if (eachProj.Dot(normals1[n1]) < minDot)		// Get minimum projection
					{
						minProj = eachProj;
						minDot = eachProj.Dot(normals1[n1]);
					}
					else if (eachProj.Dot(normals1[n1]) > maxDot)	// Get maximum projection
					{
						maxProj = eachProj;
						maxDot = eachProj.Dot(normals1[n1]);
					}
				}
			}
			sepVec1 = maxProj - minProj;	// Compute the vector from min to max

			AEVec2 total1 = maxProj;	// Total = max - min
			AEVec2 total2 = minProj;

			// Project all vertices of p1 into normal of p1
			for (int v1 = 0; v1 < 4; v1++)
			{
				eachProj = corners1[v1].Project(normals1[n1]);
				if (!v1)	// Take first projection as min and max
				{
					minProj = eachProj;
					maxProj = eachProj;
					minDot = minProj.Dot(normals1[n1]);
					maxDot = maxProj.Dot(normals1[n1]);
				}
				else
				{
					if (eachProj.Dot(normals1[n1]) < minDot)		// Get minimum projection
					{
						minProj = eachProj;
						minDot = eachProj.Dot(normals1[n1]);
					}
					else if (eachProj.Dot(normals1[n1]) > maxDot)	// Get maximum projection
					{
						maxProj = eachProj;
						maxDot = eachProj.Dot(normals1[n1]);
					}
				}
			}
			sepVec2 = maxProj - minProj;	// Compute the vector from min to max

			total1 -= minProj;	// Total = max - min
			total2 -= maxProj;

			if (total1.LengthSq() > total2.LengthSq())
				totalProj = total1;
			else
				totalProj = total2;


			float penet = sepVec1.Length() + sepVec2.Length() - totalProj.Length();
			// If any projection don't overlap, not colliding
			if (penet <= 0)
				return false;
			if (penet < minPenet)	// Update minimum penetration
			{
				minPenet = penet;
				normal = normals1[n1];
			}
		}

		AEVec2 OBB1Vec2(OBB1->mTranslation.x, OBB1->mTranslation.y);
		AEVec2 OBB2Vec2(OBB2->mTranslation.x, OBB2->mTranslation.y);
		// Compute intersection point
		for (int v2 = 0; v2 < 4; v2++)	// Vertices of OBB2 inside OBB1
		{
			if (StaticPointToOrientedRect(&corners2[v2], &OBB1Vec2, OBB1->mScale.x, OBB1->mScale.y, OBB1->mOrientation))
			{
				pResult->mPi = corners2[v2];
				break;
			}
		}
		for (int v1 = 0; v1 < 4; v1++)	// Vertices of OBB1 inside OBB2 (take this before vertices of OBB2 inside OBB1)
		{
			if (StaticPointToOrientedRect(&corners1[v1], &OBB2Vec2, OBB2->mScale.x, OBB2->mScale.y, OBB2->mOrientation))
			{
				pResult->mPi = corners1[v1];
				break;
			}
		}



		if ((OBB2Vec2 - OBB1Vec2).Dot(normal) < 0)	// Always from OBB1 to OBB2
			normal = -normal;
		pResult->mNormal = normal.Normalize();
		pResult->mPenetration = minPenet;

		return true;
	}

	// Helper function
	AEVec2 CheckVertex(AEVec2* pos1, AEVec2* size1, AEVec2* pos2, AEVec2* size2)
	{
		AEVec2 vertex(0,0);

		//Second Square
		vertex.x = pos2->x + size2->x / 2;		vertex.y = pos2->y + size2->y / 2;   //Up right
		if (StaticPointToStaticRect(&vertex, pos1, size1->x, size1->y))
			return vertex;

		vertex.x = pos2->x + size2->x / 2;		vertex.y = pos2->y - size2->y / 2;  //Down right
		if (StaticPointToStaticRect(&vertex, pos1, size1->x, size1->y))
			return vertex;

		vertex.x = pos2->x - size2->x / 2;		vertex.y = pos2->y + size2->y / 2;  //Top left
		if (StaticPointToStaticRect(&vertex, pos1, size1->x, size1->y))
			return vertex;

		vertex.x = pos2->x - size2->x / 2;		vertex.y = pos2->y - size2->y / 2;  //Down left
		if (StaticPointToStaticRect(&vertex, pos1, size1->x, size1->y))
			return vertex;

		//First square
		vertex.x = pos1->x + size1->x / 2;		vertex.y = pos1->y + size1->y / 2;   //Up right
		if (StaticPointToStaticRect(&vertex, pos2, size2->x, size2->y))
			return vertex;

		vertex.x = pos1->x + size1->x / 2;		vertex.y = pos1->y - size1->y / 2;  //Down right
		if (StaticPointToStaticRect(&vertex, pos2, size2->x, size2->y))
			return vertex;

		vertex.x = pos1->x - size1->x / 2;		vertex.y = pos1->y + size1->y / 2;  //Top left
		if (StaticPointToStaticRect(&vertex, pos2, size2->x, size2->y))
			return vertex;

		vertex.x = pos1->x - size1->x / 2;		vertex.y = pos1->y - size1->y / 2;  //Down left
		if (StaticPointToStaticRect(&vertex, pos2, size2->x, size2->y))
			return vertex;

		return vertex;
	}
}


