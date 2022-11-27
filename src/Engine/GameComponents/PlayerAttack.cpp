
#include "PlayerAttack.h"
#include <Platform/AEXInput.h>
#include "imgui/imgui.h"
#include <Engine/Platform/AEXTime.h>

namespace AEX
{
	void PlayerAttack::OnCreate()
	{
	}
	void PlayerAttack::Initialize()
	{
		// attach attack collider
		CreateChildWithGhostCollider();

		// attach rigidbody for forward thrust
		attackImpulse = mOwner->GetComp<RigidbodyComp>();
		if (attackImpulse == nullptr)
		{
			attackImpulse = mOwner->NewComp<RigidbodyComp>();
			attackImpulse->AddToSystem();
			attackImpulse->OnCreate();

			attackImpulse->mHasGravity = false;
		}
	}
	bool PlayerAttack::Edit()
	{
		return false;
	}
	void PlayerAttack::Update()
	{
		// enable attack area
		if (aexInput->KeyTriggered(Keys::KeyCodes::Space))
		{
			attackCollider->SetEnabled(true);
			attackImpulse->AddForce(thrustForce);
		}

		// attack for 1 second
		if (attackCollider->Enabled())
		{
			mCounter -= aexTime->GetFrameTime();
			// attack time ends
			if (mCounter <= 0.0f)
			{
				mCounter = 1.0f;
				attackCollider->SetEnabled(false);
			}
		}


	}
	void PlayerAttack::Shutdown()
	{
		RemoveFromSystem();
	}

	void PlayerAttack::StreamRead(const nlohmann::json& j)
	{

	}
	void PlayerAttack::StreamWrite(nlohmann::json& j) const
	{

	}

	void PlayerAttack::CreateChildWithGhostCollider()
	{
		// find attack collider in children
		const std::vector<GameObject*>& childs = mOwner->GetChildren();
		for (unsigned i = 0; i < childs.size(); ++i)
		{
			// get attack collider from child
			attackCollider = childs[i]->GetComp<Collider>();

			// check if this child has the attack collider
			if (attackCollider != nullptr && attackCollider->Ghost == true)
				break;
		}

		// if attack collider was not found, create a child and add attack collider
		if (attackCollider == nullptr || attackCollider->Ghost == false)
		{
			// create object and add as child of player
			GameObject* child = mOwner->mOwnerSpace->NewObject("attakCol");
			mOwner->AddChild(child);

			// must have a Transform
			TransformComp* tr = aexFactory->Create<TransformComp>();
			child->AddComp(tr);
			tr->mLocal.mTranslation = AEVec2(1, 0);
			tr->mLocal.mScale = AEVec2(0.5f, 0.5f);
			tr->AddToSystem();

			// must have a Collider too
			attackCollider = child->NewComp<Collider>();
			attackCollider->AddToSystem();
			child->OnCreate();
			child->Initialize();
		}

		// set Collider variables (because Alex didn't StreamRead the Transform inside Collider class)
		attackCollider->mColliderType = Collider::CT_CIRCLE;
		attackCollider->mRigidBody = nullptr;
		attackCollider->Static = false;
		attackCollider->Ghost = true;
		attackCollider->SetEnabled(false);
	}
}