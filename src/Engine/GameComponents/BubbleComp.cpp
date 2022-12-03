#include "BubbleComp.h"
#include "imgui/imgui.h"					// Edit fn
#include "Editor/AEXEditor.h"				// aexEditor
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	void BubbleComp::OnCreate()
	{
		mMaxSpeed = 5.0f;	// still to be tested
		mRgbd = nullptr;
	}
	void BubbleComp::Initialize()
	{
		// rigidbody for collisions and movement
		mRgbd = mOwner->GetComp<RigidbodyComp>();
		if (mRgbd == nullptr)
		{
			mRgbd = mOwner->NewComp<RigidbodyComp>();
			mRgbd->AddToSystem();
			mRgbd->OnCreate();
		}
		mRgbd->mHasGravity = false;

		// circle collider
		Collider* mCol = mOwner->GetComp<Collider>();
		if (mCol == nullptr)
		{
			mCol = mOwner->NewComp<Collider>();
			mCol->AddToSystem();
			mCol->OnCreate();

			mCol->mColliderType = Collider::CT_CIRCLE;
			mCol->Static = false;
		}
		mCol->mRigidBody = mRgbd;
		mCol->Ghost = false;

		// register breakable collider to CollisionStayEvent for breaking it
		//mOwner->mEvents.subscribe<CollisionStayEvent, BreakableComp>(this, &BreakableComp::Break);
	}
	void BubbleComp::Update()
	{
		// if bubble goes too far from center, make it go to center

		// clamp max speed
		//std::cout << "Updating..." << std::endl;

		// add random force in both x and y
		if (mRgbd != nullptr)
			mRgbd->AddForce( {((float)(rand() % 10) - 4.5f )/ 100.0f, ((float)(rand() % 10) - 4.5f) / 100.0f } );
	}
	void BubbleComp::Shutdown()
	{
		RemoveFromSystem();
		// register breakable collider to CollisionStayEvent for breaking it
		//mOwner->mEvents.unsubscribe(MemberFunctionHandler<BreakableComp, CollisionStayEvent>(this, &BreakableComp::Break), "struct AEX::CollisionStayEvent");
	}
	bool BubbleComp::Edit()
	{
		return true;
	}


	void BubbleComp::StreamRead(const nlohmann::json& j)
	{
		mMaxSpeed = j["max speed"];
	}
	void BubbleComp::StreamWrite(nlohmann::json& j) const
	{
		j["max speed"] = mMaxSpeed;
	}
}