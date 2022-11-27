#include <Core/AEXCore.h>
#include <Physics/RigidbodyComponent.h>
#include "AEXPhysicsSystem.h"

namespace AEX
{
	bool PhysicsSystem::Initialize()
	{
		return true;
	}
	void PhysicsSystem::Update()
	{
		// Update Rigid bodies
		for (auto it : mRigidBodies)
			if (it->GetOwner()->Enabled())
				it->Update();
	}
	void PhysicsSystem::AddRBody(RigidbodyComp* RBody)
	{
		mRigidBodies.push_back(RBody);
	}
	void PhysicsSystem::RemoveRBody(RigidbodyComp* RBody)
	{
		mRigidBodies.remove(RBody);
	}
}