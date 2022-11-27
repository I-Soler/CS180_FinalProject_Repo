#pragma once
#include "..\Core\AEXCore.h"

// ----------------------------------------------------------------------------
namespace AEX
{
	class RigidbodyComp;
	class PhysicsSystem : public IBase
	{
		AEX_RTTI_DECL(PhysicsSystem, IBase);
		AEX_SINGLETON(PhysicsSystem) {}

	public:

		bool Initialize();
		void Update();

		void AddRBody(RigidbodyComp* RBody);
		void RemoveRBody(RigidbodyComp* RBody);

		AEX_PTR_LIST(RigidbodyComp) mRigidBodies;
	protected:
		
	};
}

#define aexPhysics AEX::Singletons::Get<AEX::PhysicsSystem>()
