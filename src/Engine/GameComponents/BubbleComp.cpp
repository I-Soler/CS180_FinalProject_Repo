#include "BubbleComp.h"
#include "AEX.h"
#include "imgui/imgui.h"					// Edit fn
#include "Editor/AEXEditor.h"				// aexEditor
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include <Collisions/Raycast.h>
#include "Graphics/Components/AEXGfxComps.h"
#include "Bullet.h"
#include <mutex>

namespace AEX
{
	std::mutex mtx;

	std::list<GameObject*> BubbleComp::otherBubbles;
	std::map<TurretComp*, bool> BubbleComp::turrets;

	void BubbleComp::OnCreate()
	{
		mSpeed = 80.0f;
		mRgbd = nullptr;
		mTimer.Pause();
		otherBubbles.remove(mOwner);	// avoid dups
		otherBubbles.push_back(mOwner);
		//turrets.clear();
	}
	void BubbleComp::Initialize()
	{
		multitheaded = aexEngine->Multithreaded;
		cooldown.Start();

		mOwner->mEvents.subscribe<CollisionEnterEvent, BubbleComp>(this, &BubbleComp::Die);

		// rigidbody for collisions and movement
		mRgbd = mOwner->GetComp<RigidbodyComp>();
		if (mRgbd == nullptr)
		{
			mRgbd = mOwner->NewComp<RigidbodyComp>();
			mRgbd->AddToSystem();
			mRgbd->OnCreate();
			mRgbd->Initialize();
		}
		mRgbd->mHasGravity = false;

		// circle collider
		Collider* mCol = mOwner->GetComp<Collider>();
		if (mCol == nullptr)
		{
			mCol = mOwner->NewComp<Collider>();
			mCol->AddToSystem();
			mCol->OnCreate();
			mCol->Initialize();

			mCol->mColliderType = Collider::CT_CIRCLE;
			mCol->Static = false;
		}
		mCol->SetScale({ 0.5f, 0.5f });
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

				if (multitheaded)
					thread_ids.push_back(std::thread(Dodge, t_info));
				else
					Dodge(t_info);
			}
		}

		bool SepararLasBurbujasSiDemasiadoCerca = false;

		for (auto& it : BulletComp::AllBullets)
		{
			if (!SepararLasBurbujasSiDemasiadoCerca)
				break;
			if (cooldown.GetTimeSinceStart() < 2)
				break;

			float dist = mTr->mLocal.mTranslation.Distance(it->mLocal.mTranslation);

			if (dist < 60)		// last avoiding resort
			{
				cooldown.Reset();

				thread_info t_info;
				t_info.thisPtr = this;
				t_info.pos = { mTr->GetPosition().x, mTr->GetPosition().y };
				t_info.radius = mTr->GetScale().x;
				t_info.dir = it->GetOwner()->GetComp<BulletComp>()->dir;

				AEVec2 perpDir{ -t_info.dir.y, t_info.dir.x };
				perpDir.NormalizeThis();
				float miniRadius = t_info.radius * 0.707f;
				AEVec2 newPosLeft = t_info.pos + perpDir * 100.0f;
				AEVec2 newPosRight = t_info.pos - perpDir * 100.0f;

				Pop(t_info, miniRadius, newPosLeft, newPosRight);

				cooldown.Start();
			}
		}

		// avoid bullet moving
		if (mRgbd != nullptr)
		{
			if (dodgeMoving && mTimer.isPaused_ == true)
			{
				// move away from bullet
				mRgbd->AddForce({ mSpeed * Cos(dodgeAngle) * 20.0f, mSpeed * Sin(dodgeAngle) * 20.0f });
				mTimer.Reset();
				mTimer.Start();
			}
			// dodge for 0.5 or 1 second
			else if (mTimer.GetTimeSinceStart() >= mDodgeTime)
			{
				// reset dodge force
				mRgbd->mVelocity = { 0.0f, 0.0f, 0.0f };
				mTimer.Reset();
				mTimer.Pause();
				dodgeMoving = false;
			}
			// add random force in both x and y
			else
			{
				// keep bubble far from edges
				float x_dir = 0.0f;
				if (mTr->GetWorldPosition().x > 300.0f)
					x_dir = -80.0f;
				if (mTr->GetWorldPosition().x < -300.0f)
					x_dir = 80.0f;
				else x_dir = 80.0f * Cos((float)(rand() % 10) * TWO_PI / 10.0f);	// between 0 and 2PI

				float y_dir = 0.0f;
				if (mTr->GetWorldPosition().y > 150.0f)
					y_dir = -80.0f;
				if (mTr->GetWorldPosition().y < -150.0f)
					y_dir = 80.0f;
				else y_dir = 80.0f * Sin((float)(rand() % 10) * TWO_PI / 10.0f);	// between 0 and 2PI

				mRgbd->AddForce({ x_dir, y_dir });
			}
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
		std::string evName = typeid(CollisionEnterEvent).name();

		//mOwner->mEvents.unsubscribe(*mOwner->mEvents.AllEvents[evName][0], evName);
		RemoveFromSystem();

		otherBubbles.remove(mOwner);
	}
	bool BubbleComp::Edit()
	{
		ImGui::Checkbox("Enable bubble Dying", &DieOnContact);
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

	void BubbleComp::Die(const CollisionEnterEvent& collision)
	{
		if (!DieOnContact)
			return;
		if (collision.otherObject->GetComp<BulletComp>())	// if it has collided with a bullet
			mOwner->mOwnerSpace->DeleteObject(mOwner);
	}



	// FUNCTION READ IN MULTITHREDING
	void Dodge(thread_info ti)
	{
		/*...................Is the bubble in danger?...................*/
		AEVec2 result;	// dummy
		if (RayCastCircle(ti.origin, ti.dir, ti.pos, ti.radius + 50.0f, &result) == -1)
			return;

		/*...........Can we dodge bullet moving away from it?...........*/
		// try to dodge in every directions
		if (ti.thisPtr->dodgeMoving == false)
		{
			// 8 loops
			float initAngle = (float)(rand() % 8) * PI / 4.0f;
			for (float angle = initAngle; angle < initAngle + TWO_PI; angle += PI / 4.0f)
			{
				// check ways to avoid collision
				AEVec2 newPos(ti.pos.x + 50.0f * Cos(angle), ti.pos.y + 50.0f * Sin(angle));
				unsigned ray = RayCastCircle(ti.origin, ti.dir, newPos, ti.radius + 10.0f, &result);
				if (ray == -1)
				{
					// dodge
					ti.thisPtr->dodgeMoving = true;
					ti.thisPtr->dodgeAngle = angle;
					float& speed = ti.thisPtr->mSpeed;
					ti.thisPtr->mDodgeTime = 0.5f;
					return;
				}
			}
			// if no dodge angle was found, check for bubble far from turret to dodge moving faster
			for (float angle = initAngle; angle < initAngle + TWO_PI; angle += PI / 4.0f)
			{
				// check ways to avoid collision
				AEVec2 newPos(ti.pos.x + 100.0f * Cos(angle), ti.pos.y + 80.0f * Sin(angle));
				unsigned ray = RayCastCircle(ti.origin, ti.dir, newPos, ti.radius + 10.0f, &result);

				// this is for far bubbles, if they are close to turrets don't dodge this way
				if (ray < 600000)
					break;
				else if (ray == -1)
				{
					// dodge
					ti.thisPtr->dodgeMoving = true;
					ti.thisPtr->dodgeAngle = angle;
					ti.thisPtr->mDodgeTime = 0.8f;
					return;
				}
			}	// if no dodge angle was found, try something else
		}

		/*..........Can we avoid bullet joining to another bubble?..........*/
		// try to attach to another bubble
		if (ti.radius < MAX_RADIUS && ti.thisPtr->canJoin)
		{
			unsigned size = BubbleComp::otherBubbles.size();
			for (auto it = BubbleComp::otherBubbles.begin(); it != BubbleComp::otherBubbles.end(); ++it)
			{
				// not join to itself
				if (*it == ti.thisPtr->GetOwner()) continue;
				// restart (sanity check)
				if (BubbleComp::otherBubbles.size() != size)
				{
					Dodge(ti);
					return;
				}
				// other bubble too far away for joining
				TransformComp* otherTr = (*it)->GetComp<TransformComp>();
				if (otherTr == nullptr) return;
				// check other's distance and scale
				if ((otherTr->GetPosition() - ti.pos).LengthSq() > MAX_DIST_SQ
					|| otherTr->GetScale().x > MAX_RADIUS)
					continue;

				/* simulate what would happen if we joined this bubble */

				// midpoint between two bubbles
				AEVec2 newPos = (AEVec2(otherTr->GetPosition().x, otherTr->GetPosition().y) + ti.pos) / 2.0f;
				// radius of new joined circle
				float newRadius = (otherTr->GetScale().x + ti.radius) * 0.8f;
				// check collision
				if (RayCastCircle(ti.origin, ti.dir, newPos, newRadius + 0.0f, &result) == -1)
				{	// can dodge this way, then do it!

					if (ti.thisPtr->canJoin == false) return;	// more multithreaded sanity checks

					mtx.lock();
					// in case this other bubble tries to join us, only one join at a time
					for (auto it2 = BubbleComp::otherBubbles.begin(); it2 != BubbleComp::otherBubbles.end(); ++it2)
						if (*it2 != *it)
							(*it2)->GetComp<BubbleComp>()->canJoin = false;

					// delete other bubble
					(*it)->mOwnerSpace->DeleteObject(*it);

					// update this bubble as a union of both
					auto tr = ti.thisPtr->GetOwner()->GetComp<TransformComp>();
					tr->SetScale({ newRadius, newRadius });
					tr->SetPosition(newPos);

					BubbleComp::otherBubbles.erase(it);

					for (auto it2 = BubbleComp::otherBubbles.begin(); it2 != BubbleComp::otherBubbles.end(); ++it2)
							(*it2)->GetComp<BubbleComp>()->canJoin = true;

					mtx.unlock();
					return;
				}
			}
		}	// if this method fails, try something else

		/*.............Can we avoid bullet splitting the bubble?.............*/
		// perpendicular direction for mini twin bubbles
		AEVec2 perpDir{ -ti.dir.y, ti.dir.x };
		perpDir.NormalizeThis();
		float miniRadius = ti.radius * 0.707f;
		AEVec2 newPosLeft = ti.pos + perpDir * 100.0f;
		AEVec2 newPosRight = ti.pos - perpDir * 100.0f;

		if (ti.radius > MIN_RADIUS &&
			RayCastCircle(ti.origin, ti.dir, newPosLeft, miniRadius + 10.0f, &result) == -1 &&
			RayCastCircle(ti.origin, ti.dir, newPosRight, miniRadius + 10.0f, &result) == -1)
		{	// can dodge this way, then do it!
			Pop(ti, miniRadius, newPosLeft, newPosRight);
		}

		// pop
	}
	void Pop(thread_info& ti, float miniRadius, AEVec2 newPosLeft, AEVec2 newPosRight)
	{
		// update this bubble as mini left bubble
		auto tr = ti.thisPtr->GetOwner()->GetComp<TransformComp>();
		tr->SetScale({ miniRadius, miniRadius });
		tr->SetPosition(newPosLeft);

		// create mini right bubble
		mtx.lock();

		GameObject* Obj = ti.thisPtr->GetOwnerSpace()->NewObject("bubble");	// create the bubble
		tr = aexFactory->Create<TransformComp>();
		tr->SetScale({ miniRadius, miniRadius });
		tr->SetPosition(newPosRight);
		Obj->AddComp(tr);
		Obj->NewComp<Renderable>()->AddToSystem();
		auto bc = Obj->NewComp<BubbleComp>();
		bc->AddToSystem();
		bc->OnCreate();
		bc->Initialize();
		bc->DieOnContact = ti.thisPtr->DieOnContact;
		Obj->OnCreate();
		Obj->Initialize();

		mtx.unlock();
	}
}