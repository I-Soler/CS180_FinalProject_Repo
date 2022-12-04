#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Turret.h"
#include "Bullet.h"
#include "BubbleComp.h"
#include "Graphics/Components/AEXGfxComps.h"
#include "imgui/imgui.h"

namespace AEX
{
	void TurretComp::OnCreate()
	{
	}
	void TurretComp::Initialize()
	{
		timer.Start();
		RotTimer.Start();
		ParentTr = mOwner->GetComp<TransformComp>();

		BubbleComp::turrets[this] = false;
	}
	void TurretComp::Update()
	{
		if (RotClockWise)
		{
			ParentTr->mLocal.mOrientation += 0.01;
			if (RotTimer.GetTimeSinceStart() > 2)
			{
				RotClockWise = false;
				RotTimer.Reset();
			}
		}
		else
		{
			ParentTr->mLocal.mOrientation -= 0.01;
			if (RotTimer.GetTimeSinceStart() > 2)
			{
				RotClockWise = true;
				RotTimer.Reset();
			}
		}


		if (timer.GetTimeSinceStart() >= 5 + shootDelay)	// make a bullet each 5 seconds
			Shoot();
		
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

		BubbleComp::turrets[this] = true;

		timer.Reset();
	}

	void TurretComp::StreamRead(const nlohmann::json& j)
	{
		shootDelay = j["shootDelay"];
	}

	void TurretComp::StreamWrite(nlohmann::json& j) const
	{
		j["shootDelay"] = shootDelay;
	}

	void TurretComp::Shutdown()
	{
		RemoveFromSystem();
	}
	bool TurretComp::Edit()
	{
		ImGui::DragFloat("Delay", &shootDelay, 0, 4);
		return false;
	}
}