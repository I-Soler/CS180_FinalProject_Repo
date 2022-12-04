#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Turret.h"
#include "Bullet.h"
#include "Graphics/Components/AEXGfxComps.h"

namespace AEX
{
	AEVec2 TurretComp::lastBulletPos;
	AEVec2 TurretComp::lastBulletDir;

	void TurretComp::OnCreate()
	{
	}
	void TurretComp::Initialize()
	{
		timer.Start();

		ParentTr = mOwner->GetComp<TransformComp>();
	}
	void TurretComp::Update()
	{
		if (timer.GetTimeSinceStart() >= 5)	// make a bullet each 5 seconds
		{
			Space* mainSp = aexScene->GetMainSpace();				// Get space where object will be added

			GameObject* Obj = mainSp->NewObject("bullet");	// create the object

			// Every object must have a Transform and a Renderable by default
			TransformComp* tr = aexFactory->Create<TransformComp>();

			tr->mLocal.mScale = AEVec2(10, 10);
			tr->mLocal.mTranslation = ParentTr->mWorld.mTranslation;
			tr->mLocal.mOrientation = ParentTr->mWorld.mOrientation;
			Obj->AddComp(tr);	Obj->NewComp<BulletComp>();
			Obj->NewComp<Renderable>();
			Obj->OnCreate(); Obj->Initialize();

			lastBulletPos = tr->mLocal.mTranslation;
			lastBulletDir = { Cos(tr->mLocal.mOrientation + PI / 2.0f), Sin(tr->mLocal.mOrientation + PI / 2.0f) };

			timer.Reset();
		}
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