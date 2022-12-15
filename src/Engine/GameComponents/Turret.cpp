#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Turret.h"
#include "Bullet.h"
#include "BubbleComp.h"
#include "Graphics/Components/AEXGfxComps.h"
#include "imgui/imgui.h"
#include <iostream>
#include "ResourceMgr/ResourceMgr.h"

namespace AEX
{
	void TurretComp::OnCreate()
	{
		timer.Start();
	}
	void TurretComp::Initialize()
	{
		timer.Reset();
		ParentTr = mOwner->GetComp<TransformComp>();

		BubbleComp::turrets[this] = false;
	}
	void TurretComp::Update()
	{
		if (BubbleComp::otherBubbles.size() == 0)
			return; 

		BubbleComp::turrets[this] = false;

		if (timer.GetTimeSinceStart() >= Recharge + shootDelay)	// make a bullet each 5 seconds
		{
			int randomBubble = (rand() % BubbleComp::otherBubbles.size());	// get random buble to shoot at

			int pos = 0;
			AEVec2 dir;
			for (auto it : BubbleComp::otherBubbles)	// search bubble
			{
				if (pos == randomBubble)
				{
					dir = it->GetComp<TransformComp>()->mLocal.mTranslation - ParentTr->mLocal.mTranslation;
					break;
				}
				pos++;
			}
		
			dir.NormalizeThis();

			ParentTr->mLocal.mOrientation = atan2(dir.y, dir.x) - (PI/2);	// Look at bubble
			shootDelay = 0;	// delay only affects first shot
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
		tr->mLocal.mTranslation = ParentTr->mLocal.mTranslation;
		tr->mLocal.mOrientation = ParentTr->mLocal.mOrientation;
		Obj->AddComp(tr);	Obj->NewComp<BulletComp>()->gun = this;
		auto ren = Obj->NewComp<Renderable>();
		ren->mpTexture = aexResources->GetResource<Texture>("bullet.png");
		auto col = Obj->NewComp<Collider>();
		col->Ghost = true;
		col->mColliderType = Collider::CT_CIRCLE;
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
		ImGui::DragFloat("Initial Delay", &shootDelay, 0.1, 0,4);
		ImGui::DragFloat("Time between bullets", &Recharge, 0.1, 0.5, 8);
		return false;
	}
}