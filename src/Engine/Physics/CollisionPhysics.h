#ifndef COLLISION_H_
#define COLLISION_H_
// ---------------------------------------------------------------------------
#include "Collisions/Collisions.h"
#include "Collisions/ContactCollisions.h"
#include "RigidBody.h"
#include "aexmath/AEXMath.h"

// Collision restitution for velocity resolution
#define DFLT_COLLISION_ITERATIONS 5
#define DFLT_COLLISION_RESTITUTION 0.908f

namespace AEX
{

	bool CollideCircles(RigidBody* body1, RigidBody* body2, Contact* c);
	bool CollideAABBs(RigidBody* body1, RigidBody* body2, Contact* c);
	bool CollideOBBs(RigidBody* body1, RigidBody* body2, Contact* c);
	bool CollideAABBToCircle(RigidBody* body1, RigidBody* body2, Contact* c);
	bool CollideOBBToCircle(RigidBody* body1, RigidBody* body2, Contact* c);
	// typedef for function pointer CollisionFn
	typedef bool(*CollisionFn)(RigidBody*, RigidBody*, Contact*);


	struct CollisionPhysics
	{
		// ------------------------------------------------------------------------
		// Member Variables
		std::list<RigidBody*> mStaticBodies;
		std::list<RigidBody*> mDynamicBodies;

		// Configurable parameter: Collision iterations [1, +inf(
		u32 mCollisionIterations;

		// Configurable parameter: Restitution Coefficient. 
		// Note(Thomas): This parameter should really be stored separately so that
		// it can be edited on a per-object basis. 
		f32 mCollisionRestitution;

		// Debug statistics: collisions this frame
		u32 mCollisionsThisFrame;

		// Collision Tests -They are added to the collision system at initialize. 
		// (see CollisionSystem::Init) for more details.
		CollisionFn mCollisionTests[CSHAPE_INDEX_MAX];

		// ------------------------------------------------------------------------
		// Member Functions

		// System Functions
		void Init();
		void Update();
		void Shutdown();

		// Rigid Body Management
		void AddRigidBody(RigidBody* obj, bool is_dynamic);
		void RemoveRigidBody(RigidBody* obj);
		void ClearBodies();

		// finding the collision tests
		CollisionFn GetCollisionFn(RigidBody* b1, RigidBody* b2);

		// @TODO
		// Collides and resolve all rigidbodies 
		void CollideAllBodies();

		// @TODO
		// Exposed solver
		void ResolveContactPenetration(RigidBody* obj1, RigidBody* obj2, Contact* contact);
		void ResolveContactVelocity(RigidBody* obj1, RigidBody* obj2, Contact* contact);

		// ------------------------------------------------------------------------
		// On Demand API

		// Ray casting
		struct RaycastResult {
			RigidBody* mBody;
			f32			mImpactTime;
			AEVec2		mImpactPoint;
			typedef std::list<RaycastResult> LIST;
			bool operator<(const RaycastResult& rhs)const {
				return mImpactTime < rhs.mImpactTime;
			}
		};
		f32 RaycastWithStatics(const Ray& ray, RaycastResult::LIST& outList);
		f32 RaycastWithDynamics(const Ray& ray, RaycastResult::LIST& outList);
		f32 RaycastWithAll(const Ray& ray, RaycastResult::LIST& outList);

		// Collision
		bool CollideBodyWithStatics(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts = NULL);
		bool CollideBodyWithDynamics(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts = NULL);
		bool CollideBodyWithAll(RigidBody* body, OBJECT_PTR_LIST& outList, std::list<Contact>* pContacts = NULL);

	};

	extern CollisionSystem gCollisionSystem;

	// ---------------------------------------------------------------------------
#endif
}

