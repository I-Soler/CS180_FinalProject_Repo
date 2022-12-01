#include "EnemyHealth.h"
#include <GameComponents/PlayerAttack.h>	// access player's attackCollider
#include <Physics/RigidbodyComponent.h>
#include "imgui/imgui.h"

namespace AEX
{
	void EnemyHealth::OnCreate()
	{
	}

	void EnemyHealth::Initialize()
	{
		// mEvents subscribe to attackCollider (from player's collider)
		// and to CollisionEnterEvent to take damage

		// attach rigidbody for forward thrust
		enemyPhy = mOwner->GetComp<RigidbodyComp>();
		if (enemyPhy == nullptr)
		{
			enemyPhy = mOwner->NewComp<RigidbodyComp>();
			enemyPhy->AddToSystem();
			enemyPhy->OnCreate();
			enemyPhy->Initialize();

			enemyPhy->mHasGravity = false;
			enemyPhy->mDrag = 0.9f;
		}

		// attach collider
		enemyBox = mOwner->GetComp<Collider>();
		if (enemyBox == nullptr || enemyBox->Ghost == true)
		{
			enemyBox = mOwner->NewComp<Collider>();
			enemyBox->AddToSystem();
			enemyBox->OnCreate();
			enemyBox->Initialize();

			enemyBox->mColliderType = Collider::CT_AABB;
			enemyBox->mRigidBody = enemyPhy;
			enemyBox->Static = false;
			enemyBox->Ghost = false;
			//enemyBox->SetPosition(AEVec3(0, 0, 0));
			//enemyBox->SetScale(AEVec2(1, 1));
			//enemyBox->SetOrientation(0.0f);
		}

		// register enemy collider to CollisionStayEvent for calling GetHurt
		mOwner->mEvents.subscribe<CollisionStayEvent, EnemyHealth>(this, &EnemyHealth::GetHurt);
	}

	void EnemyHealth::Update()
	{
	}

	void EnemyHealth::Shutdown()
	{
		mOwner->mEvents.unsubscribe<CollisionStayEvent, EnemyHealth>(this, &EnemyHealth::GetHurt);
	}

	bool EnemyHealth::Edit()
	{
		return false;
	}

	void EnemyHealth::GetHurt(const CollisionStayEvent& col)
	{
		// must be player and ghost collider (attackCollider)
		if (col.otherObject->mTag != GameObject::PLAYER_TAG || col.Contact.obj[1]->Ghost == false)
			return;

		// get damage
		enemyHealth -= 10.0f;
		enemyPhy->AddForce(AEVec2(1000.0f, 0));

		if (enemyHealth <= 0.0f)
		{
			// destroy object
		}
	}

	void EnemyHealth::StreamRead(const nlohmann::json& j)
	{
	}

	void EnemyHealth::StreamWrite(nlohmann::json& j) const
	{
	}
}
