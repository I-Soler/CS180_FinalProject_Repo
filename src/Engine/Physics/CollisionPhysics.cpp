#include "CollisionPhysics.h"

// ----------------------------------------------------------------------------
#pragma region // @PROVIDED

namespace AEX
{
	CollisionPhysics gCollisionSystem;



	void CollisionPhysics::Init()
	{
		// default collision iterations and restitution
		mCollisionIterations = DFLT_COLLISION_ITERATIONS;
		mCollisionRestitution = DFLT_COLLISION_RESTITUTION;

		for (u32 i = 0; i < CSHAPE_INDEX_MAX; ++i)
			mCollisionTests[i] = NULL;

		mCollisionTests[CSHAPE_CIRCLE | CSHAPE_CIRCLE] = CollideCircles;
		mCollisionTests[CSHAPE_AABB | CSHAPE_AABB] = CollideAABBs;
		mCollisionTests[CSHAPE_OBB | CSHAPE_OBB] = CollideOBBs;
		mCollisionTests[CSHAPE_CIRCLE | CSHAPE_AABB] = CollideAABBToCircle;
		mCollisionTests[CSHAPE_OBB | CSHAPE_AABB] = CollideOBBs;
		mCollisionTests[CSHAPE_CIRCLE | CSHAPE_OBB] = CollideOBBToCircle;
	}


	void CollisionPhysics::Update()
	{
		CollideAllBodies();
	}


	void CollisionPhysics::Shutdown()
	{
		ClearBodies();
	}


	void CollisionPhysics::AddRigidBody(RigidBody* obj, bool is_dynamic)
	{
		if (is_dynamic)
			mDynamicBodies.push_back(obj);
		else
			mStaticBodies.push_back(obj);
	}


	void CollisionPhysics::RemoveRigidBody(RigidBody* obj)
	{
		mDynamicBodies.remove(obj);
		mStaticBodies.remove(obj);
	}


	void CollisionPhysics::ClearBodies()
	{
		mDynamicBodies.clear();
		mStaticBodies.clear();
	}


	CollisionFn CollisionPhysics::GetCollisionFn(RigidBody* b1, RigidBody* b2)
	{
		u32 collision_index = b1->mCollisionShape | b2->mCollisionShape;
		return mCollisionTests[collision_index];
	}

	bool CollideCircles(RigidBody* body1, RigidBody* body2, Contact* c)
	{
		Vector2 pos1 = body1->mTransform.mPosition;
		Vector2 pos2 = body2->mTransform.mPosition;

		return StaticCircleToStaticCircleEx(&pos1, body1->mTransform.mScale.x, &pos2, body2->mTransform.mScale.x, c);
	}
	bool CollideAABBs(RigidBody* body1, RigidBody* body2, Contact* c)
	{
		Vector2 p1 = body1->mTransform.mPosition, s1 = body1->mTransform.mScale,
			p2 = body2->mTransform.mPosition, s2 = body2->mTransform.mScale;
		return StaticRectToStaticRectEx(&p1, &s1, &p2, &s2, c);
	}
	bool CollideOBBs(RigidBody* body1, RigidBody* body2, Contact* c)
	{
		Transform2D obb1(body1->mTransform.mPosition, body1->mTransform.mScale, body1->mTransform.mOrientation);
		Transform2D obb2(body2->mTransform.mPosition, body2->mTransform.mScale, body2->mTransform.mOrientation);
		return OrientedRectToOrientedRectEx(&obb1, &obb2, c);
	}
	bool CollideAABBToCircle(RigidBody* body1, RigidBody* body2, Contact* c)
	{
		// which is which
		RigidBody* rect = body1->mCollisionShape == CSHAPE_AABB ? body1 : body2;
		RigidBody* circle = body1->mCollisionShape == CSHAPE_CIRCLE ? body1 : body2;

		if (StaticRectToStaticCircleEx(&Vector2(rect->mTransform.mPosition), rect->mTransform.mScale.x, rect->mTransform.mScale.y, &Vector2(circle->mTransform.mPosition), circle->mTransform.mScale.x, c))
		{
			if (circle == body1) // flip normal to match our convention
				c->mNormal = -c->mNormal;
			return true;
		}
		return false;
	}
	bool CollideOBBToCircle(RigidBody* body1, RigidBody* body2, Contact* c)
	{

		// which is which
		Transform2D obb = body1->mCollisionShape == CSHAPE_OBB
			? Transform2D(body1->mTransform.mPosition, body1->mTransform.mScale, body1->mTransform.mOrientation)
			: Transform2D(body2->mTransform.mPosition, body2->mTransform.mScale, body2->mTransform.mOrientation);

		RigidBody* circle = body1->mCollisionShape == CSHAPE_CIRCLE ? body1 : body2;
		if (StaticOBBToStaticCircleEx(&obb, &Vector2(circle->mTransform.mPosition), circle->mTransform.mScale.x, c))
		{
			if (circle == body1) // flip normal to match our convention
				c->mNormal = -c->mNormal;
			return true;
		}
		return false;
	}

#pragma endregion
	// ----------------------------------------------------------------------------

	// ----------------------------------------------------------------------------
#pragma region // @TODO


	void CollisionPhysics::ResolveContactPenetration(RigidBody* body1, RigidBody* body2, Contact* contact)
	{
		float TotalInvMass = body1->mInvMass + body2->mInvMass;
		float MassInf1 = body1->mInvMass / TotalInvMass;
		float MassInf2 = body2->mInvMass / TotalInvMass;

		body1->mTransform.mPosition = body1->mTransform.mPosition - contact->mNormal * contact->mPenetration * MassInf1;
		body2->mTransform.mPosition = body2->mTransform.mPosition + contact->mNormal * contact->mPenetration * MassInf2;
	}


	void CollisionPhysics::ResolveContactVelocity(RigidBody* body1, RigidBody* body2, Contact* contact)
	{
		float TotalInvMass = body1->mInvMass + body2->mInvMass;
		float MassInf1 = body1->mInvMass / TotalInvMass;
		float MassInf2 = body2->mInvMass / TotalInvMass;

		AEVec2 r = body2->mVelocity - body1->mVelocity;
		float sep1 = r * contact->mNormal;
		float sep2 = -sep1 * mCollisionRestitution;
		float sepTotal = sep2 - sep1;

		if (sep1 > 0)   // Objects aleady separating, dont resolve
			return;
		else
		{
			body1->mVelocity = body1->mVelocity - contact->mNormal * sepTotal * MassInf1 * MassInf1;
			body2->mVelocity = body2->mVelocity + contact->mNormal * sepTotal * MassInf2 * MassInf2;
		}
	}


	void CollisionSystem::CollideAllBodies()
	{
		bool col = false;
		mCollisionsThisFrame = 0;

		for (int it = 0; it < mCollisionIterations; it++)
		{
			for (auto it1 = mDynamicBodies.begin(); it1 != mDynamicBodies.end(); it1++)   //Dyn-vs-Dyn
			{
				for (auto it2 = mDynamicBodies.begin(); it2 != mDynamicBodies.end(); it2++)
				{
					if (it1 == it2)  // dont check collision with itself
						continue;

					Contact contact;
					col = GetCollisionFn(*it1, *it2)(*it1, *it2, &contact);   // Get collision data

					if (col == true)
					{
						if (it == 0)  // resolve velocity once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(*it1, *it2, &contact);
						}

						ResolveContactPenetration(*it1, *it2, &contact);
					}

				}
			}

			for (auto it1 = mDynamicBodies.begin(); it1 != mDynamicBodies.end(); it1++)  //  Dyn-vs-Static
			{
				for (auto it2 = mStaticBodies.begin(); it2 != mStaticBodies.end(); it2++)
				{
					Contact contact;
					col = GetCollisionFn(*it1, *it2)(*it1, *it2, &contact); // Get collision data

					if (col == true)
					{
						if (it == 0)  // resolve velocity once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(*it1, *it2, &contact);
						}

						ResolveContactPenetration(*it1, *it2, &contact);
					}
				}
			}
		}
	}



	f32 CollisionPhysics::RaycastWithStatics(const Ray& ray, RaycastResult::LIST& outList)
	{
		float contact = 0;

		for (auto it = mStaticBodies.begin(); it != mStaticBodies.end(); it++)    //Check with every object
		{
			Vector2 outPi;

			switch ((**it).mCollisionShape)
			{
			case(CSHAPE_CIRCLE):
			{
				contact = RayCastCircle(ray, (*it)->mTransform.mPosition, (*it)->mTransform.mScale.x, &outPi);
				break;
			}

			default:   // AABB or OOBB, is the same
			{
				Transform2D object;
				object.mOrientation = (**it).mTransform.mOrientation, object.mPosition = (**it).mTransform.mPosition, object.mScale = (**it).mTransform.mScale;

				contact = RayCastRect(ray, object, &outPi);
				break;
			}
			}

			if (contact >= 0)   // Contact detected
			{
				RaycastResult result{ *it, contact, outPi };
				outList.push_back(result);
			}
		}

		if (!outList.empty())	// If intersection detected
		{
			outList.sort();
			return outList.front().mImpactTime;
		}

		return -1.0f;	//return -1 (no impact)
	}


	f32 CollisionPhysics::RaycastWithDynamics(const Ray& ray, RaycastResult::LIST& outList)
	{
		float contact = 0;

		for (auto it = mDynamicBodies.begin(); it != mDynamicBodies.end(); it++)    //Check with every object
		{
			Vector2 outPi;

			switch ((**it).mCollisionShape)
			{
			case(CSHAPE_CIRCLE):
			{
				contact = RayCastCircle(ray, (*it)->mTransform.mPosition, (*it)->mTransform.mScale.x, &outPi);
				break;
			}

			default:   // AABB or OOBB, is the same
			{
				Transform2D object;
				object.mOrientation = (**it).mTransform.mOrientation, object.mPosition = (**it).mTransform.mPosition, object.mScale = (**it).mTransform.mScale;

				contact = RayCastRect(ray, object, &outPi);
				break;
			}
			}

			if (contact >= 0)   // Contact detected
			{
				RaycastResult result{ *it, contact, outPi };
				outList.push_back(result);
			}
		}

		if (!outList.empty())	// If intersection detected
		{
			outList.sort();
			return outList.front().mImpactTime;
		}

		return -1.0f;	//return -1 (no impact)
	}


	f32 CollisionPhysics::RaycastWithAll(const Ray& ray, RaycastResult::LIST& outList)
	{
		outList.clear();
		RaycastWithStatics(ray, outList);
		RaycastWithDynamics(ray, outList);
		outList.sort();

		return outList.front().mImpactTime;
	}



	bool CollisionPhysics::CollideBodyWithStatics(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts)
	{
		bool col = false;
		bool added = false;
		Contact contact;

		for (int i = 0; i < mCollisionIterations; i++)
		{
			for (auto it = mStaticBodies.begin(); it != mStaticBodies.end(); it++)   //Dyn-vs-Dyn
			{
				if (*it == body)	// Prevent from colliding to itself
					continue;
				if (pContacts == NULL)
				{
					col = GetCollisionFn(body, *it)(body, *it, NULL);  // get collision

					if (col == true)
					{
						if (i == 0)   // resolve once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(body, *it, NULL);
						}
						ResolveContactPenetration(body, *it, NULL);  // resolve always

						if (added == false)
						{
							outList.push_back(body);
							added = true;
						}
					}
				}
				else
				{
					col = GetCollisionFn(body, *it)(body, *it, &contact);  // get collision

					if (col == true)
					{
						if (i == 0)   // resolve once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(body, *it, &contact);
						}
						ResolveContactPenetration(body, *it, &contact);  // resolve always
						pContacts->push_back(contact);

						if (added == false)
						{
							outList.push_back(body);
							added = true;
						}
					}
				}

			}
		}

		return col;
	}


	bool CollisionPhysics::CollideBodyWithDynamics(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts)
	{
		bool col = false;
		bool added = false;
		Contact contact;

		for (int i = 0; i < mCollisionIterations; i++)
		{
			for (auto it = mDynamicBodies.begin(); it != mDynamicBodies.end(); it++)   //Dyn-vs-Dyn
			{
				if (*it == body)	// Prevent from colliding to itself
					continue;
				if (pContacts == NULL)
				{
					col = GetCollisionFn(body, *it)(body, *it, NULL);  // get collision

					if (col == true)
					{
						if (i == 0)   // resolve once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(body, *it, NULL);
						}
						ResolveContactPenetration(body, *it, NULL);  // resolve always

						if (added == false)
						{
							outList.push_back(body);
							added = true;
						}
					}
				}
				else
				{
					col = GetCollisionFn(body, *it)(body, *it, &contact);  // get collision

					if (col == true)
					{
						if (i == 0)   // resolve once
						{
							mCollisionsThisFrame++;
							ResolveContactVelocity(body, *it, &contact);
						}
						ResolveContactPenetration(body, *it, &contact);  // resolve always
						pContacts->push_back(contact);

						if (added == false)
						{
							outList.push_back(body);
							added = true;
						}
					}
				}

			}
		}

		return col;
	}


	bool CollisionPhysics::CollideBodyWithAll(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts)
	{
		bool col1 = CollideBodyWithStatics(body, outList, pContacts);
		bool col2 = CollideBodyWithDynamics(body, outList, pContacts);

		if (col1 == true || col2 == true)
			return true;
		else
			return false;
	}

#pragma endregion
	// ----------------------------------------------------------------------------
}
