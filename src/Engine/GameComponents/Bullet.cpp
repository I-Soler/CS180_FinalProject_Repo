#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Bullet.h"
#include "BubbleComp.h"
#include "Turret.h"
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	std::list<TransformComp*> BulletComp::AllBullets;

	void BulletComp::OnCreate()
	{
	}
	void BulletComp::Initialize()
	{
		timer.Start();
		tr = mOwner->GetComp<TransformComp>();
		dir = AEVec2(2.5 * sin(-tr->mLocal.mOrientation), 2.5 * cos(-tr->mLocal.mOrientation));

		AllBullets.push_back(tr);
	}
	void BulletComp::Update()
	{
		tr->Translate(dir * 1.5);

		if (timer.GetTimeSinceStart() >= 5)	// make bullet die in 5 seconds
		{
			// this turret is no longer shooting
			BubbleComp::turrets[gun] = false;
			mOwner->mOwnerSpace->DeleteObject(mOwner);
		}
	}
	void BulletComp::Shutdown()
	{
		AllBullets.remove(tr);
		RemoveFromSystem();
	}
	bool BulletComp::Edit()
	{
		return false;
	}
}