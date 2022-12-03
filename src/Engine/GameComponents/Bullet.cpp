#include "imgui/imgui.h"					// Edit fn
#include <cstdlib>							// rand
#include <Collisions/AEXCollisionSystem.h>	// Collider
#include "Bullet.h"
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
		dir = AEVec2(1.5 * sin(-tr->mLocal.mOrientation), 1.5 * cos(-tr->mLocal.mOrientation));
	}
	void BulletComp::Update()
	{
		tr->Translate(dir);

		if (timer.GetTimeSinceStart() >= 8)	// make bullet die in 10 seconds
		{
			Space* space = mOwner->mOwnerSpace;
			space->DeleteObject(mOwner);
		}
	}
	void BulletComp::Shutdown()
	{
		RemoveFromSystem();
	}
	bool BulletComp::Edit()
	{
		return false;
	}
}