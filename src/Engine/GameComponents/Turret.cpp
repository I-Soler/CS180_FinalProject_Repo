#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Turret.h"
#include "Bullet.h"
#include "BubbleComp.h"
#include "Graphics/Components/AEXGfxComps.h"

namespace AEX
{
	void TurretComp::OnCreate()
	{
	}
	void TurretComp::Initialize()
	{
		timer.Pause();
		ParentTr = mOwner->GetComp<TransformComp>();

		// add turret and set shooting false
		BubbleComp::turrets[this] = false;
		shootDelay = BubbleComp::turrets.size() * SHOOT_MARGIN + 5.0f;
	}
	void TurretComp::Update()
	{
		// wait a bit before shooting first time
		if (!firstShootDone)
		{
			if (timer.isPaused_)
				timer.Start();
			else if (timer.GetTimeSinceStart() >= shootDelay)
			{
				Shoot();
				firstShootDone = true;
			}
		}

		if (timer.GetTimeSinceStart() >= 5)	// make a bullet each 5 seconds
		{
			Shoot();
		}
	}
	void TurretComp::Shoot()
	{
		Space* mainSp = aexScene->GetMainSpace();				// Get space where object will be added

		GameObject* Obj = mainSp->NewObject("bullet");	// create the object

		// Every object must have a Transform and a Renderable by default
		TransformComp* tr = aexFactory->Create<TransformComp>();

		tr->mLocal.mScale = AEVec2(10, 10);
		tr->mLocal.mTranslation = ParentTr->mWorld.mTranslation;
		tr->mLocal.mOrientation = ParentTr->mWorld.mOrientation;
		Obj->AddComp(tr);	Obj->NewComp<BulletComp>()->gun = this;
		Obj->NewComp<Renderable>();
		Obj->OnCreate(); Obj->Initialize();

		bulletPos = tr->mLocal.mTranslation;
		bulletDir = { Cos(tr->mLocal.mOrientation + PI / 2.0f), Sin(tr->mLocal.mOrientation + PI / 2.0f) };

		// this shot done true
		BubbleComp::turrets[this] = true;

		timer.Reset();
	}

	void TurretComp::Shutdown()
	{
		RemoveFromSystem();
	}
	bool TurretComp::Edit()
	{
		return false;
	}
}