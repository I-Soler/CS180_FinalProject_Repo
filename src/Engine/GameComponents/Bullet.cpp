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
	}
	void BulletComp::Update()
	{
		
		tr->mLocal.mTranslation.y += 0.1;

		if (timer.GetTimeSinceStart() >= 6)	// make bullet die in 10 seconds
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