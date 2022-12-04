#include "BubbleComp.h"
#include "imgui/imgui.h"					// Edit fn
#include "Editor/AEXEditor.h"				// aexEditor
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include <Collisions/Raycast.h>
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	std::list<GameObject*> BubbleComp::otherBubbles;
	std::map<TurretComp*, bool> BubbleComp::turrets;

	void BubbleComp::OnCreate()
	{
		mSpeed = 80.0f;
		mRgbd = nullptr;
		mTimer.Pause();
		otherBubbles.push_back(mOwner);
		turrets.clear();
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
		if (thread_ids.empty())
		{
			// dodge every shooting turrets
			for (auto it = turrets.begin(); it != turrets.end(); ++it)
			{
				if (it->second == false) continue;

				// create thread
				thread_info t_info;
				t_info.thisPtr = this;
				t_info.pos = { mTr->GetPosition().x, mTr->GetPosition().y };
				t_info.radius = mTr->GetScale().x;
				t_info.origin = it->first->bulletPos;
				t_info.dir = it->first->bulletDir;

				thread_ids.push_back(std::thread(Dodge, t_info));
			}
		}
		// avoid bullet moving
		if (mRgbd != nullptr)
		{
			if (dodgeMoving && mTimer.isPaused_ == true)
			{
				// move away from bullet
				mRgbd->AddForce({ mSpeed * Cos(dodgeAngle) * 30.0f, mSpeed * Sin(dodgeAngle) * 30.0f });
				mTimer.Reset();
				mTimer.Start();
			}
			// dodge for 0.5 seconds
			else if (mTimer.GetTimeSinceStart() >= 0.5f)
			{
				// reset dodge force
				mRgbd->mVelocity = { 0.0f, 0.0f, 0.0f };
				mTimer.Reset();
				mTimer.Pause();
				dodgeMoving = false;
			}
		}

		// add random force in both x and y
		else if (mRgbd != nullptr)
		{
			float x_dir = mSpeed * Cos((float)(rand() % 10) * TWO_PI / 10.0f) / 10.0f;	// between 0 and 2PI
			float y_dir = mSpeed * Sin((float)(rand() % 10) * TWO_PI / 10.0f) / 10.0f;	// between 0 and 2PI
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

		// register breakable collider to CollisionStayEvent for breaking it
		//mOwner->mEvents.unsubscribe(MemberFunctionHandler<BreakableComp, CollisionStayEvent>(this, &BreakableComp::Break), "struct AEX::CollisionStayEvent");
		RemoveFromSystem();
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

	// FUNCTION READ IN MULTITHREDING
	void Dodge(thread_info ti)
	{
		/*..........Is the bubble in danger?..........*/
		AEVec2 result;	// dummy
		if (RayCastCircle(ti.origin, ti.dir, ti.pos, ti.radius + 20.0f, &result) == -1)
			return;

		/*..........Can we dodge bullet moving away from it?..........*/
		// try to dodge in every directions
		if (ti.thisPtr->dodgeMoving == false)
		{
			// 8 loops
			float angle = 0.0f;
			for (angle; angle < TWO_PI; angle += PI / 4.0f)
			{
				// check ways to avoid collision
				AEVec2 newPos(ti.pos.x + 30.0f * Cos(angle), ti.pos.y + 30.0f * Sin(angle));
				if (RayCastCircle(ti.origin, ti.dir, newPos, ti.radius, &result) == -1)
				{
					// dodge
					ti.thisPtr->dodgeMoving = true;
					ti.thisPtr->dodgeAngle = angle;
					return;
				}
			}	// if no dodge angle was found, try something else
		}

		/*..........Can we avoid bullet joining to another bubble?..........*/
		// try to attach to another bubble
		if(ti.thisPtr->canJoin)
		for (auto it = BubbleComp::otherBubbles.begin(); it != BubbleComp::otherBubbles.end(); ++it)
		{
			// not join to itself
			if (*it == ti.thisPtr->GetOwner()) continue;
			// other bubble too far away for joining
			TransformComp* otherTr = (*it)->GetComp<TransformComp>();
			if (otherTr == nullptr) return;
			if ((otherTr->GetPosition() - ti.pos).LengthSq() >= MAX_DIST_SQ) continue;

			// simulate what would happen if we joined this bubble 1.41

			// midpoint between two bubbles
			AEVec2 newPos = (AEVec2(otherTr->GetPosition().x, otherTr->GetPosition().y) + ti.pos) / 2.0f;
			// radius of new joined circle
			float newRadius = (otherTr->GetScale().x + ti.radius) * 0.8f;
			// check collision
			if (RayCastCircle(ti.origin, ti.dir, newPos, newRadius + 0.0f, &result) != -1)
			{	// can dodge this way, then do it!

				if (ti.thisPtr->canJoin == false) return;	// more multithreaded sanity checks

				// in case this other bubble tries to join us
				(*it)->GetComp<BubbleComp>()->canJoin = false;

				// delete other bubble
				if (ti.thisPtr->canJoin == false) return;	// more multithreaded sanity checks
				(*it)->mOwnerSpace->DeleteObject(*it);

				// update this bubble as a union of both
				auto tr = ti.thisPtr->GetOwner()->GetComp<TransformComp>();
				tr->SetScale({ newRadius, newRadius });
				tr->SetPosition(newPos);

				BubbleComp::otherBubbles.erase(it);
				break;
			}
		}

		// pop
	}
}