#include "BubbleComp.h"
#include "imgui/imgui.h"					// Edit fn
#include "Editor/AEXEditor.h"				// aexEditor
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include <Collisions/Raycast.h>
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	bool BubbleComp::shotDone = false;

	void BubbleComp::OnCreate()
	{
		mSpeed = 5.0f;	// still to be tested
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

		// transform
		mTr = mOwner->GetComp<TransformComp>();

		// register breakable collider to CollisionStayEvent for breaking it
		//mOwner->mEvents.subscribe<CollisionStayEvent, BreakableComp>(this, &BreakableComp::Break);
	}
	void BubbleComp::Update()
	{
		// clear previous threads
		if (!thread_ids.empty())
		{
			for (auto it = thread_ids.begin(); it != thread_ids.end(); ++it)
			{
				it->join();
			}
			//thread_infos.clear();
			thread_ids.clear();
		}

		// if bubble goes too far from center, make it go to center

		// clamp max speed

		// check collision if a shot was done
		if (shotDone && thread_ids.empty())
		{
			// create thread
			thread_info t_info;
			t_info.thisPtr = this;
			t_info.pos = { mTr->GetPosition().x, mTr->GetPosition().y };
			t_info.radius = mTr->GetScale().x;
			//thread_infos.push_back(t_info);
			thread_ids.push_back(std::thread(Dodge, t_info));
		}

		if (dodgeMoving)
		{
			if (!shotDone)
				dodgeMoving = false;
			if (mRgbd != nullptr)
			{
				// move away from bullet
				mRgbd->AddForce({ mSpeed * Cos(dodgeAngle), mSpeed * Sin(dodgeAngle) });
			}
		}

		// add random force in both x and y
		else if (mRgbd != nullptr)
		{
			float x_dir = mSpeed * Cos((float)(rand() % 10) * TWO_PI / 10.0f);	// between 0 and 2PI
			float y_dir = mSpeed * Sin((float)(rand() % 10) * TWO_PI / 10.0f);	// between 0 and 2PI
			mRgbd->AddForce( { x_dir, y_dir } );
		}
	}
	void BubbleComp::Shutdown()
	{
		// clear all threads
		if (!thread_ids.empty())
		{
			for (auto it = thread_ids.begin(); it != thread_ids.end(); ++it)
				it->join();
			thread_ids.clear();
		}
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
		mSpeed = j["speed"];
	}
	void BubbleComp::StreamWrite(nlohmann::json& j) const
	{
		j["speed"] = mSpeed;
	}

	void Dodge(thread_info ti)
	{
		AEVec2 result;	// dummy
		float angle = 0.0f;
		// try to dodge in every directions
		for (angle = 0.0f; angle < TWO_PI; angle += 0.1f)
		{
			AEVec2 newPos(ti.origin.x + 10.0f * Cos(angle), ti.origin.y + 10.0f * Sin(angle));
			if (RayCastCircle(ti.origin, ti.dir, newPos, ti.radius, &result) == -1)
			{
				// dodge
				ti.thisPtr->dodgeMoving = true;
				ti.thisPtr->dodgeAngle = angle;
				return;
			}
		}

		// try to attach to another bubble

		// pop
	}
}