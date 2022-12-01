#include "Graphics/AEXGfxSystem.h"
#include <Physics/RigidbodyComponent.h>
#include "AEXCollisionSystem.h"
#include "ContactCollisions.h"
#include <Extern/imgui/imgui.h>
#include "MessagingSystem/EventList.h"


namespace AEX     // For the Collider class
{
	Collider::Collider()
	{
		mOwner = NULL;
		mColliderType = CT_OBB;
		mRigidBody = NULL;
		Static = false;
		Ghost = true;
	}
	Collider::Collider(ColliderType ColliderType, bool isStatic, bool isGhost, GameObject* Owner, RigidbodyComp* RigidBody)
	{
		mColliderType = ColliderType;
		Static = isStatic;
		Ghost = isGhost;
		mRigidBody = RigidBody;
		mOwner = Owner;
			
	}
	void Collider::AddToSystem()
	{	
		aexCollision->AddCol(this);
	}
	void Collider::RemoveFromSystem()
	{
		aexCollision->RemoveCol(this);
	}
	void Collider::OnCreate()
	{
		mParentTransform = mOwner->GetComp<TransformComp>();
		//AddToSystem();		 quiza hay que hacer esto
	}
	void Collider::Shutdown()
	{
		RemoveFromSystem();
	}
	void Collider::Update()
	{
		// If rigidBody is created after collider, add it here
		if (mRigidBody == nullptr && mOwner->GetComp<RigidbodyComp>() != nullptr)
			mRigidBody = mOwner->GetComp<RigidbodyComp>();

		// If rigidbody is deleted, become null
		if (mOwner->GetComp<RigidbodyComp>() == nullptr)
			mRigidBody = nullptr;

		// just do the world update
		TransformComp::Update();

		// Should not be necesary, comented in case it is
		// Object can't be Non Ghost and Non Static without RigidBody
		//if (!Ghost && !Static && mRigidBody == NULL)
		//	Ghost = true;
	}
	bool Collider::Edit()
	{
		DrawColliders();

		Update();

		// divide by 8 so the editor shows the correct RadioButton
		int e = mColliderType / 8;	
		ImGui::Text("Shape");
		ImGui::RadioButton("Circle", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("AABB", &e, 1);

		float scaleTr[2] = { mLocal.mScale.x,mLocal.mScale.y };
		float translationTr[2] = { mLocal.mTranslation.x,mLocal.mTranslation.y };
		//float orientationTr = mLocal.mOrientation;

		ImGui::InputFloat2("Offset", translationTr);
		ImGui::InputFloat2("Size", scaleTr);		
		//ImGui::InputFloat("Orientation", &orientationTr);

		SetScale(AEVec2(scaleTr[0], scaleTr[1]));
		SetPosition(AEVec2(translationTr[0], translationTr[1]));
		//SetOrientation(orientationTr);

		switch (e)
		{
		case 0:
			mColliderType = CT_CIRCLE;
			break;
		case 1:
			mColliderType = CT_OBB;
			break;
		}

		bool BeforeCheckStatic = Static;

		ImGui::Checkbox("Ghost", &Ghost);
		ImGui::Checkbox("Static", &Static);

		// Object can't be Non Ghost and Non Static without RigidBody
		if (!Ghost && !Static && mRigidBody == nullptr)
			Ghost = true;


		// A collider can change if it's static on update
		// If this happens, it needs to be removed from it's old Colision list
		// and get added to it's corresponding one
		if (Static != BeforeCheckStatic)
		{
			Static = BeforeCheckStatic;
			aexCollision->RemoveCol(this);
			Static = !BeforeCheckStatic;
			aexCollision->AddCol(this);
		}

		return true;
	}
	void Collider::StreamRead(const nlohmann::json& j)
	{
		std::string tmp;

		tmp = j["Owner"];
		if (tmp == "null")
			mOwner = NULL;

		tmp = j["RigidBody"];
		if (tmp == "null")
			mRigidBody = NULL;
		else
			mRigidBody = mOwner->GetComp<RigidbodyComp>();

		mColliderType = j["ColliderType"];
		Static = j["Static"];
		Ghost = j["Ghost"];

		TransformComp::StreamRead(j);
	}
	void Collider::StreamWrite(nlohmann::json& j) const
	{
		j["Owner"] = mOwner? mOwner->GetName() : "null";
		j["RigidBody"] = mRigidBody? mRigidBody->GetName() : "null";

		j["ColliderType"] = mColliderType;
		j["Static"] = Static;
		j["Ghost"] = Ghost;

		TransformComp::StreamWrite(j);
	}

	void Collider::DrawColliders()
	{
		switch (mColliderType)
		{
		case AEX::Collider::CT_POINT:
			break;	
			
		case AEX::Collider::CT_AABB:
		{
			aexGraphics->DrawRect(GetWorldPosition().x, GetWorldPosition().y, GetWorldScale().x, GetWorldScale().y, Color(0, 1, 0, 1));
			break;
		}
		case AEX::Collider::CT_CIRCLE:
		{
			aexGraphics->DrawCircle(GetWorldPosition().x, GetWorldPosition().y, GetWorldScale().x, Color(0, 1, 0, 1));
			break;
		}
		case AEX::Collider::CT_OBB:
		{
			aexGraphics->DrawOrientedRect(GetWorldPosition().x, GetWorldPosition().y, GetWorldScale().x, GetWorldScale().y, GetWorldOrientation(), Color(0, 1, 0, 1));
			break;
		}
		case AEX::Collider::CT_LINELIST:
			break;
		}	
	}

	ContactInfo::ContactInfo()
	{
		obj[0] = NULL;
		obj[1] = NULL;
		mNormal = AEVec2(0, 0);
		mImpactPoint = AEVec2(0, 0);
		mPenetration = 0;
	}
	ContactInfo::ContactInfo(Collider* obj1_, Collider* obj2_, AEVec2 mNormal_, AEVec2 mImpactPoint_, f32 mPenetration_)
	{
		obj[0] = obj1_;
		obj[1] = obj2_;
		mNormal = mNormal_;
		mImpactPoint = mImpactPoint_;
		mPenetration = mPenetration_;
	}
}

namespace AEX     // For the Collision System class
{
	bool CollisionSystem::Initialize()
	{
		// Set the function map
		mCollisionLibrary[Collider::CT_POINT | Collider::CT_CIRCLE] = PointvsCircle; 
		mCollisionLibrary[Collider::CT_POINT | Collider::CT_AABB] = PointvsAABB; 
		mCollisionLibrary[Collider::CT_POINT | Collider::CT_OBB] = PointvsOBB; 

		mCollisionLibrary[Collider::CT_CIRCLE | Collider::CT_CIRCLE] = CirclevsCircle;  
		mCollisionLibrary[Collider::CT_OBB | Collider::CT_CIRCLE] = CirclevsOBB;   

		mCollisionLibrary[Collider::CT_AABB | Collider::CT_AABB] = AABBvsAABB;
		mCollisionLibrary[Collider::CT_AABB | Collider::CT_CIRCLE] = AABBvsCircle;
		mCollisionLibrary[Collider::CT_AABB | Collider::CT_OBB] = AABBvsOBB;

		mCollisionLibrary[Collider::CT_OBB | Collider::CT_OBB] = OBBvsOBB;

		// So the list is not empty
		mFrameContacts = &mContacts[0];
		mPrevContacts = &mContacts[1];

		return true;
	}

	void CollisionSystem::Update()
	{
		// Update colliders
		for (auto it : mDynamicCollider)
			if (it->GetOwner()->Enabled())
				it->Update();
		
		for (auto it : mStaticCollider)
			if (it->GetOwner()->Enabled())
				it->Update();
		

		// Get collisions
		mFrameContacts->clear();
		NarrowPhaseCollision(*mFrameContacts);
		ResolveCollision(*mFrameContacts);		

		// Broadcast
		BroadCast(*mFrameContacts);

		// swap pointers
		auto tmp = mPrevContacts;
		mPrevContacts = mFrameContacts;
		mFrameContacts = tmp;
	}

	void CollisionSystem::AddCol(Collider* col)
	{
		RemoveCol(col);		// remove duplicates

		if (col->Static == true)
			mStaticCollider.push_back(col);
		else
			mDynamicCollider.push_back(col);
	}

	void CollisionSystem::RemoveCol(Collider* col)
	{
		if (col->Static == true)
			mStaticCollider.remove(col);
		else
			mDynamicCollider.remove(col);
	}

	void CollisionSystem::NarrowPhaseCollision(std::list<ContactInfo>& CollisionList)
	{
		ContactInfo info ;

		for (auto& d1 : mDynamicCollider)
		{
			if (!d1->GetOwner()->Enabled() || !d1->Enabled())
				continue;

			for (auto & d2 : mDynamicCollider)
			{
				if (!d2->GetOwner()->Enabled() || !d2->Enabled())
					continue;

				if (d1 == d2 || d1->GetOwner() == d2->GetOwner()) // ignore itself and same owner
					continue;
				
				if (d1->GetOwner()->mOwnerSpace != d2->GetOwner()->mOwnerSpace) // Ignore other spaces
					continue;

				if (mCollisionLibrary[d1->mColliderType | d2->mColliderType](d1, d2, &info))
					CollisionList.push_back(ContactInfo(d1, d2, info.mNormal, info.mImpactPoint, info.mPenetration));
			}

			for (auto &s : mStaticCollider)
			{
				if (!s->GetOwner()->Enabled())
					continue;

				if (d1->GetOwner()->mOwnerSpace != s->GetOwner()->mOwnerSpace) // Ignore other spaces
					continue;

				if (mCollisionLibrary[d1->mColliderType | s->mColliderType](d1, s, &info))
					CollisionList.push_back(ContactInfo(d1, s, info.mNormal, info.mImpactPoint, info.mPenetration));
			}
		}
	}

	void CollisionSystem::ResolveCollision(std::list<ContactInfo>& Contacts)
	{
		f32 mCollisionRestitution = DFLT_COLLISION_RESTITUTION;
		float TotalInvMass = 1, MassInf1 = 1, MassInf2 = 1;

		for (auto &it : Contacts)
		{
			// dont resolve contact for ghost collisions
			if (it.obj[0]->Ghost || it.obj[1]->Ghost)
				continue;

			// Get inv masses
			TotalInvMass = 1, MassInf1 = 1, MassInf2 = 1;

			// Both objects DONT have rigid Body
			if (it.obj[0]->mRigidBody == NULL && it.obj[1]->mRigidBody == NULL)
				continue;

			// Both objects have rigid Body
			else if (it.obj[0]->mRigidBody != NULL && it.obj[1]->mRigidBody != NULL)
			{
				TotalInvMass = it.obj[0]->mRigidBody->mInvMass + it.obj[1]->mRigidBody->mInvMass;
				MassInf1 = it.obj[0]->mRigidBody->mInvMass / TotalInvMass;
				MassInf2 = it.obj[1]->mRigidBody->mInvMass / TotalInvMass;
			}		

			// Only one has rigid body
			else if (it.obj[0]->mRigidBody != NULL && it.obj[1]->mRigidBody == NULL)
			{
				TotalInvMass = it.obj[0]->mRigidBody->mInvMass;
				MassInf1 = it.obj[0]->mRigidBody->mInvMass / TotalInvMass;
			}	

			// Only one has rigid body
			else if (it.obj[0]->mRigidBody == NULL && it.obj[1]->mRigidBody != NULL)
			{
				TotalInvMass = it.obj[1]->mRigidBody->mInvMass;
				MassInf2 = it.obj[1]->mRigidBody->mInvMass / TotalInvMass;
			}
				
			// Contact Penetration
			{
				// dont move object if static
				if(!it.obj[0]->Static)
					it.obj[0]->mRigidBody->mObjTr->mLocal.mTranslation = it.obj[0]->mRigidBody->mObjTr->mLocal.mTranslation + it.mNormal * it.mPenetration * MassInf1;

				if (!it.obj[1]->Static)
					it.obj[1]->mRigidBody->mObjTr->mLocal.mTranslation = it.obj[1]->mRigidBody->mObjTr->mLocal.mTranslation - it.mNormal * it.mPenetration * MassInf2;
			}

			// Contact velocity
			{
				AEVec2 r;
				
				// Check for rigidBody
				if (it.obj[0]->mRigidBody == NULL)
				{
					r = it.obj[1]->mRigidBody->mVelocity;
				}
				if (it.obj[1]->mRigidBody == NULL)
				{
					r = it.obj[0]->mRigidBody->mVelocity;
				}
				if(it.obj[0]->mRigidBody != NULL && it.obj[1]->mRigidBody != NULL)
					r = it.obj[0]->mRigidBody->mVelocity - it.obj[1]->mRigidBody->mVelocity;

				float sep1 = r * it.mNormal;
				float sep2 = -sep1 * mCollisionRestitution;
				float sepTotal = sep2 - sep1;

				if (sep1 > 0)   // Objects aleady separating, dont resolve
					return;
				else
				{
					if (!it.obj[0]->Static)
						it.obj[0]->mRigidBody->mVelocity = it.obj[0]->mRigidBody->mVelocity + it.mNormal * sepTotal * MassInf1 * MassInf1;

					if (!it.obj[1]->Static)
						it.obj[1]->mRigidBody->mVelocity = it.obj[1]->mRigidBody->mVelocity - it.mNormal * sepTotal * MassInf2 * MassInf2;
				}
			}
		}
	}

	void CollisionSystem::BroadCast(std::list<ContactInfo>& Contacts)
	{
		// step 1: broadcast the new collisions
		// loop through the new frame contacts
		for (auto& it : Contacts)
		{
			auto it2 = std::find(mPrevContacts->begin(), mPrevContacts->end(), it);

			// doesn't exists -> new collision
			if (it2 == mPrevContacts->end()) {
				// trigger CollisionStartedEvent
				
				// distributed event system

				it.obj[0]->GetOwner()->mEvents.trigger_event(CollisionEnterEvent(it.obj[1]->GetOwner(), it));
				it.obj[1]->GetOwner()->mEvents.trigger_event(CollisionEnterEvent(it.obj[0]->GetOwner(), it));
			}

			// contact exists -> collision persisted
			else
			{
				// trigger collision persisted event
				it.obj[0]->GetOwner()->mEvents.trigger_event(CollisionStayEvent(it.obj[1]->GetOwner(), it));
				it.obj[1]->GetOwner()->mEvents.trigger_event(CollisionStayEvent(it.obj[0]->GetOwner(), it));

				// remove from prevContacts
				mPrevContacts->erase(it2);
			}
		}

		// all the remaining contacts in prevContacts didn't occur again this frame 
		for (auto& it : *mPrevContacts)
		{
			it.obj[0]->GetOwner()->mEvents.trigger_event(CollisionExitEvent(it.obj[1]->GetOwner(), it));
			it.obj[1]->GetOwner()->mEvents.trigger_event(CollisionExitEvent(it.obj[0]->GetOwner(), it));
		}

		/*
			loop to find the new contact in the previous frame contact
		*/
	}

	void CollisionSystem::clearContacts()
	{
		mPrevContacts->clear();
	}

	bool PointvsAABB(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Collider* Point = c1->mColliderType == Collider::CT_POINT ? c1 : c2;
		Collider* AABB = c1->mColliderType == Collider::CT_POINT ? c2 : c1;
		Contact ExContact;

		bool col = StaticPointToStaticAABBEx(&AEVec2(Point->mWorld.mTranslation.x, c1->mWorld.mTranslation.y),
			&AEVec2(AABB->mWorld.mTranslation.x, AABB->mWorld.mTranslation.y),
			AABB->mWorld.mScale.x, AABB->mWorld.mScale.y, &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}
	bool PointvsOBB(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Collider* Point = c1->mColliderType == Collider::CT_POINT ? c1 : c2;
		Collider* OBB = c1->mColliderType == Collider::CT_POINT ? c2 : c1;
		Contact ExContact;

		bool col = StaticPointToStaticOBBEx(&AEVec2(Point->mWorld.mTranslation.x, c1->mWorld.mTranslation.y),
			&OBB->mWorld, &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}
	bool PointvsCircle(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Collider* Point = c1->mColliderType == Collider::CT_POINT ? c1 : c2;
		Collider* Circle = c1->mColliderType == Collider::CT_POINT ? c2 : c1;
		Contact ExContact;

		bool col = StaticPointToStaticCircleEx(&AEVec2(Point->mWorld.mTranslation.x, c1->mWorld.mTranslation.y),
			&AEVec2(Circle->mWorld.mTranslation.x, Circle->mWorld.mTranslation.y),
			Circle->mWorld.mScale.x, &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}

	bool CirclevsCircle(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Contact ExContact;

		bool col = StaticCircleToStaticCircleEx(&AEVec2(c1->mWorld.mTranslation.x, c1->mWorld.mTranslation.y), c1->mWorld.mScale.x,
			&AEVec2(c2->mWorld.mTranslation.x, c2->mWorld.mTranslation.y), c2->mWorld.mScale.x, &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}

	bool CirclevsOBB(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Contact ExContact;
		Collider* OBB = c1->mColliderType == Collider::CT_OBB ? c1 : c2;
		Collider* Circle = c1->mColliderType == Collider::CT_OBB ? c2 : c1;

		bool col = StaticOBBToStaticCircleEx(&OBB->mWorld, &AEVec2(Circle->mWorld.mTranslation.x, Circle->mWorld.mTranslation.y), Circle->mWorld.mScale.x, &ExContact);

		output->mImpactPoint = ExContact.mPi;

		// Swap normal 
		if (c1->mColliderType == Collider::CT_OBB)
			output->mNormal = -ExContact.mNormal;
		else
			output->mNormal = ExContact.mNormal;

		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}

	bool AABBvsAABB(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Contact ExContact;

		bool col = StaticRectToStaticRectEx(&AEVec2(c1->mWorld.mTranslation.x, c1->mWorld.mTranslation.y),
			&AEVec2(c1->mWorld.mScale.x, c1->mWorld.mScale.y),
			&AEVec2(c2->mWorld.mTranslation.x, c2->mWorld.mTranslation.y),
			&AEVec2(c2->mWorld.mScale.x, c2->mWorld.mScale.y), &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = -ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}
	bool AABBvsCircle(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Collider* AABB = c1->mColliderType == Collider::CT_AABB ? c1 : c2;
		Collider* Circle = c1->mColliderType == Collider::CT_AABB ? c2 : c1;
		Contact ExContact;

		bool col = StaticRectToStaticCircleEx(&AEVec2(AABB->mWorld.mTranslation.x, AABB->mWorld.mTranslation.y),
			AABB->mWorld.mScale.x, AABB->mWorld.mScale.y,
			&AEVec2(Circle->mWorld.mTranslation.x, Circle->mWorld.mTranslation.y),
			Circle->mWorld.mScale.x, &ExContact);

		output->mImpactPoint = ExContact.mPi;

		// Swap normal 
		if (c1->mColliderType == Collider::CT_AABB)
			output->mNormal = -ExContact.mNormal;
		else
			output->mNormal = ExContact.mNormal;

		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}


	// OBB collider not working yets
	bool AABBvsOBB(Collider* c1, Collider* c2, ContactInfo* output)
	{
		Collider* AABB = c1->mColliderType == Collider::CT_AABB ? c1 : c2;
		Collider* OBB = c1->mColliderType == Collider::CT_AABB ? c2 : c1;
		Contact ExContact;

		bool col = OrientedRectToOrientedRectEx(&AABB->mWorld, &OBB->mWorld, & ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}

	bool OBBvsOBB(Collider* c2, Collider* c1, ContactInfo* output)
	{
		Contact ExContact;

		bool col = OrientedRectToOrientedRectEx(&c1->World(), &c2->World(), &ExContact);

		output->mImpactPoint = ExContact.mPi;
		output->mNormal = ExContact.mNormal;
		output->mPenetration = ExContact.mPenetration;
		output->obj[0] = c1; output->obj[1] = c2;

		return col;
	}
}