#pragma once
#include "..\Core\AEXCore.h"
#include "Composition/TransformComponent.h"
#include <thread>

// ----------------------------------------------------------------------------
#define DFLT_COLLISION_RESTITUTION 0.908f
namespace AEX
{
	class TransformComp; 
	class RigidbodyComp;

	class Collider : public TransformComp 
	{
		AEX_RTTI_DECL(Collider, TransformComp);

	public:
		enum ColliderType
		{
			CT_POINT = 1, CT_AABB = 2, CT_CIRCLE = 4, CT_OBB = 8,
			CT_LINELIST = 16
		};

		// Default constructor
		Collider();

		//Non default constructor
		Collider(ColliderType ColliderType, bool isStatic, bool isGhost, GameObject* Owner = NULL, RigidbodyComp* RigidBody = NULL);

		// Add and remove itself from collision
		void AddToSystem() override;
		void RemoveFromSystem() override;

		// Called when object is created
		void OnCreate() override;

		void Shutdown() override;

		// Update it's world transform
		void Update() override;

		// Edit with ImGui
		bool Edit() override;

		// Serialization
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j)const override;

		// Debug function
		void DrawColliders();

		ColliderType mColliderType;
		RigidbodyComp* mRigidBody;
		bool Static;
		bool Ghost;
	};

	struct ContactInfo
	{
		Collider* obj[2];
		AEVec2 mNormal;
		AEVec2 mImpactPoint;
		f32 mPenetration;

		ContactInfo();
		ContactInfo(Collider *obj1_, Collider *obj2_, AEVec2 mNormal_, AEVec2 mImpactPoint_, f32 mPenetration_);

		bool operator==(const ContactInfo& rhs)
		{
			return (obj[0] == rhs.obj[0] 
				&& obj[1] == rhs.obj[1]) 
				|| (obj[0] == rhs.obj[1]
				&& obj[1] == rhs.obj[0]);
		}
	};

	class CollisionSystem : public IBase
	{
		AEX_RTTI_DECL(CollisionSystem, IBase);
		AEX_SINGLETON(CollisionSystem) {}

	public:
		typedef bool(*CollisionFn)(Collider*, Collider*, ContactInfo*);
		
		bool Initialize();
		void Update();

		void AddCol(Collider* col);
		void RemoveCol(Collider* col);
		
		void NarrowPhaseCollision(std::list<ContactInfo> & Contacts);
		void ResolveCollision(std::list<ContactInfo>& Contacts);
		void BroadCast(std::list<ContactInfo>& Contacts);

		void MultithreadedCollisionChecking(std::list<ContactInfo>* CollisionList, Collider* d1);

		void clearContacts();

		bool multithreaded;
	protected:
		std::vector<std::thread> thread_ids;

		// Rigid bodies
		AEX_PTR_LIST(Collider) mDynamicCollider;
		AEX_PTR_LIST(Collider) mStaticCollider;

		std::unordered_map<u32, CollisionFn> mCollisionLibrary;

		std::list<ContactInfo> mContacts[2];
		std::list<ContactInfo> *mFrameContacts, *mPrevContacts;
	};

	// collision proxies
	bool PointvsAABB(Collider* c1, Collider* c2, ContactInfo* output);
	bool PointvsOBB(Collider* c1, Collider* c2, ContactInfo* output);
	bool PointvsCircle(Collider* c1, Collider* c2, ContactInfo* output);

	bool CirclevsCircle(Collider* c1, Collider* c2, ContactInfo* output);
	bool CirclevsOBB(Collider* c1, Collider* c2, ContactInfo* output);
	
	bool AABBvsAABB(Collider* c1, Collider* c2, ContactInfo* output);
	bool AABBvsCircle(Collider* c1, Collider* c2, ContactInfo* output);
	bool AABBvsOBB(Collider* c1, Collider* c2, ContactInfo* output);

	bool OBBvsOBB(Collider* c1, Collider* c2, ContactInfo* output);


	struct CollisionEnterEvent : public Event
	{
		CollisionEnterEvent(GameObject* other, const ContactInfo& contact)
		{
			otherObject = other;
			Contact = contact;
		}

		GameObject* otherObject;
		ContactInfo Contact;
	};
	struct CollisionStayEvent : public CollisionEnterEvent
	{
		CollisionStayEvent(GameObject* other, const ContactInfo& contact) :
			CollisionEnterEvent(other, contact)
		{}

	};
	struct CollisionExitEvent : public CollisionEnterEvent
	{
		CollisionExitEvent(GameObject* other, const ContactInfo& contact) :
			CollisionEnterEvent(other, contact)
		{}

	};

}

#define aexCollision AEX::Singletons::Get<AEX::CollisionSystem>()