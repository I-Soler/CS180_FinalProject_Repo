
#include "DetachSoul.h"
#include "../GameComponents/PlayerMovement.h"				// create PlayerMovement
#include "Audio/AudioSourceComp.h"		// get AudioSourceComp
#include <Platform/AEXInput.h>			// 'space' -> detach soul
#include "imgui/imgui.h"				// Edit
#include <ResourceMgr/ResourceMgr.h>	// get EasyVictory.mp3
#include "Graphics\AEXGraphics.h"		// create Renderable
#include <Collisions/AEXCollisionSystem.h>
#include <MessagingSystem/AEXMessagingSystem.h>
namespace AEX
{
	void DetachSoul::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void DetachSoul::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void DetachSoul::Initialize()
	{
		soul = mOwner->mOwnerSpace->FindObjectByName("Soul");
		if (soul != nullptr)
			soul->SetEnabled(false);

		auto coll = mOwner->GetComp<Collider>();
		//auto handler = new MemberFunctionHandler<DetachSoul, CollisionStartedEvent>(this, &DetachSoul::OnCollisionStarted);
		coll->mEvents.subscribe(this, &DetachSoul::OnCollisionStarted);
	}
	void DetachSoul::Update()
	{
		if (!aexInput->KeyPressed(Keys::KeyCodes::Space))
			return;

		// create soul if it doesn't exist
		if (soul == nullptr)
		{
			soul = mOwner->mOwnerSpace->NewObject("Soul");
			TransformComp* tr = soul->NewComp<TransformComp>();
			tr->AddToSystem();
			tr->SetScale(AEVec2(100, 100));
			tr->SetOrientation(0.0f);
			soul->NewComp<Renderable>()->AddToSystem();
			soul->NewComp<PlayerMovement>()->AddToSystem();
		}

		// place soul in front of player
		soul->GetComp<TransformComp>()->SetPosition(mOwner->GetComp<TransformComp>()->GetPosition() + AEVec3(0, 200, 0));
		soul->OnCreate();
		soul->Initialize();
		soul->SetEnabled(true);

		// play sound effect
		mOwner->GetComp<AudioSourceComp>()->PlaySingleShot(aexResources->GetResource<Sound>("EasyVictory.mp3"));

		// desable this (later will have to disable only movement, since the player can still be rendered and harmed by enemies)
		mOwner->SetEnabled(false);
	}
	void DetachSoul::Shutdown()
	{
		aexLogic->RemoveComp(this);
	}

	bool DetachSoul::Edit()
	{
		if (ImGui::CollapsingHeader("Detach Soul"))
		{
			return true;
		}
		return false;
	}
	void DetachSoul::OnCollisionStarted(const CollisionEnterEvent& e)
	{
	}
	void DetachSoul::StreamRead(const nlohmann::json& j)
	{
		//mSpeed = j["speed"];
	}
	void DetachSoul::StreamWrite(nlohmann::json& j) const
	{
		//j["speed"] = mSpeed;
	}
}