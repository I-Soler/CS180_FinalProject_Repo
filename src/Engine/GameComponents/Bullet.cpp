#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Bullet.h"
#include "BubbleComp.h"
#include "Turret.h"
//#include <Graphics/AEXTexture.h>				// Texture

namespace AEX
{
	void BulletComp::OnCreate()
	{
	}
	void BulletComp::Initialize()
	{
		timer.Start();
		tr = mOwner->GetComp<TransformComp>();
		dir = AEVec2(2.5 * sin(-tr->mLocal.mOrientation), 2.5 * cos(-tr->mLocal.mOrientation));
	}
	void BulletComp::Update()
	{
		// remember to bubbles that there is still at least 1 bullet on screen
		BubbleComp::shotDone = true;

		tr->Translate(dir);

		if (timer.GetTimeSinceStart() >= 5)	// make bullet die in 5 seconds
		{
			Space* space = mOwner->mOwnerSpace;
			space->DeleteObject(mOwner);
		}
	}
	void BulletComp::Shutdown()
	{
		RemoveFromSystem();
		BubbleComp::shotDone = false;
	}
	bool BulletComp::Edit()
	{
		return false;
	}
}