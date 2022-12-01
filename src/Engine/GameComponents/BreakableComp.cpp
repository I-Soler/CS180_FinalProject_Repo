#include "BreakableComp.h"
#include "imgui/imgui.h"						// Edit fn
#include "Editor/AEXEditor.h"					// aexEditor
#include <Physics/RigidbodyComponent.h>			// RigidbodyComp
#include <Graphics/AEXTexture.h>				// Texture
//#include <Graphics/Components/AEXGfxComps.h>

namespace AEX
{
	void BreakableComp::OnCreate()
	{
	}
	void BreakableComp::Initialize()
	{
		// rigidbody for collisions
		RigidbodyComp* rgbd = mOwner->GetComp<RigidbodyComp>();
		if (rgbd == nullptr)
		{
			rgbd = mOwner->NewComp<RigidbodyComp>();
			rgbd->AddToSystem();
			rgbd->OnCreate();
		}
		rgbd->mHasGravity = false;

		// collider -> non-ghost first, ghost after breaking object
		Collider* mCol = mOwner->GetComp<Collider>();
		if (mCol == nullptr)
		{
			mCol = mOwner->NewComp<Collider>();
			mCol->AddToSystem();
			mCol->OnCreate();

			mCol->mColliderType = Collider::CT_AABB;
			mCol->Static = true;
		}
		mCol->mRigidBody = rgbd;
		mCol_ghost = &mCol->Ghost;
		*mCol_ghost = false;

		// register breakable collider to CollisionStayEvent for breaking it
		mOwner->mEvents.subscribe<CollisionStayEvent, BreakableComp>(this, &BreakableComp::Break);
	}
	void BreakableComp::Update()
	{}
	void BreakableComp::Shutdown()
	{
		// register breakable collider to CollisionStayEvent for breaking it
		mOwner->mEvents.unsubscribe(MemberFunctionHandler<BreakableComp, CollisionStayEvent>(this, &BreakableComp::Break), "struct AEX::CollisionStayEvent");
	}
	bool BreakableComp::Edit()
	{
		// set texture that will have after breaking object
		if (ImGui::CollapsingHeader("Set broken texture")
			&& aexEditor->getResource<Texture>(&mBrokenTex))
		{
			return true;
		}
		// set texture that will have after breaking object
		else if (ImGui::CollapsingHeader("Breakable reacts with object with this tag:"))
		{
			auto& tagMap = aexEditor->tags;	// map of tag numbers and strings

			// show react object tag
			ImGui::Text("react tag:");
			ImGui::SameLine();
			ImGui::Text(tagMap[mReactTag].c_str());

			// set react object tag
			if (ImGui::CollapsingHeader("Set react tag"))
			for (std::map<unsigned, std::string>::iterator it = tagMap.begin(); it != tagMap.end(); ++it)
				if (ImGui::Button(it->second.c_str()))
					mReactTag = it->first;
		}
		// set broken bool
		else if (ImGui::CollapsingHeader("Broken:"))
		{
			// show broken bool
			ImGui::SameLine();
			mBroken ? ImGui::Text("true") : ImGui::Text("false");

			// change broken bool
			if (ImGui::Button("change broken bool"))
				mBroken = !mBroken;
		}
		return true;
	}

	void BreakableComp::Break(const CollisionStayEvent& col)
	{
		// must be player  0: player // 1: rock
		if (col.otherObject->mTag != mReactTag || col.Contact.obj[0]->Ghost == false)
			return;

		if (!mBroken && col.Contact.obj[1]->Ghost == false)
		{
			*mCol_ghost = true;
			mBroken = true;
			// change texture
			mOwner->GetComp<Renderable>()->SetTexture(mBrokenTex);
		}
		else if (mBroken && col.Contact.obj[0]->Ghost == true)
		{
			Act();
		}
	}

	void BreakableComp::Act()
	{}

	void BreakableComp::StreamRead(const nlohmann::json& j)
	{
		std::string tmp = j["broken texture"];
		if (tmp != "null")
			mBrokenTex = aexResources->GetResource<Texture>(tmp.c_str());
		mBroken = j["is broken"];
		mReactTag = j["react tag"];
	}
	void BreakableComp::StreamWrite(nlohmann::json& j) const
	{
		j["broken texture"] = mBrokenTex ? mBrokenTex->GetName() : "null";
		j["is broken"] = mBroken;
		j["react tag"] = mReactTag;
	}
}