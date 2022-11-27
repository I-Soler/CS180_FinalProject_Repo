#include <Core/AEXCore.h>
#include <Scene/Scene.h>
#include "Composition/AEXGameObject.h"
#include "TransformComponent.h"
#include <Graphics/AEXGfxSystem.h>
#include <Platform/AEXSerialization.h>
#include <Extern/imgui/imgui.h>
#include <Extern/imgui/ImGuizmo.h>

namespace AEX
{
	TransformComp::TransformComp()
	{}
	TransformComp::~TransformComp()
	{}
	void TransformComp::AddToSystem()
	{
		this->mOwner->mOwnerSpace->AddTransform(this);
	}
	void TransformComp::RemoveFromSystem()
	{
		this->mOwner->mOwnerSpace->RemoveTransform(this);
	}
	void TransformComp::OnCreate()
	{
		mParentTransform = mOwner->mParent->GetComp<TransformComp>();
		
		if (mParentTransform)
		{
			mParentTransform->AddChildTransform(this);
			//mParentTransform->mChildren.push_back(this);
		}

		// get the children transforms from the object's children objects
		for (const auto cObj : mOwner->GetChildren())
			if (auto tr = cObj->GetComp<TransformComp>())
				mChildren.push_back(tr);
	}
	void TransformComp::Update()
	{
		if (mParentTransform)
			mWorld = mParentTransform->mWorld * mLocal;
		else
			mWorld = mLocal;

		// update the children
		for(auto c : mChildren)
			c->Update();	
	}

	bool TransformComp::Edit()
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::DragFloat3("translation", mLocal.mTranslation.v);
			ImGui::DragFloat2("Scale", mLocal.mScale.v);

			// Convert to degrees before showing, then reconvert to radians if it changed
			float orientationTr = RadToDeg(mLocal.mOrientation);
			if (ImGui::DragFloat("Orientation", &orientationTr))
				mLocal.mOrientation = DegToRad(orientationTr);


			return true;
		}
		return false;
	}

	void TransformComp::Shutdown()
	{
		if (mParentTransform)
			RemoveChildTransform(this);

		RemoveFromSystem();
	}

	void TransformComp::AddChildTransform(TransformComp* tr)
	{
		for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
			if (*it == tr)
				return;
		mChildren.push_back(tr);
	}
	void TransformComp::RemoveChildTransform(TransformComp* tr)
	{
		// todo
		for (std::list<TransformComp*>::iterator it = mParentTransform->mChildren.begin();
			it != mParentTransform->mChildren.end(); ++it)
			if (*it == tr)
			{
				mParentTransform->mChildren.erase(it);
				break;
			}
	}

	// Settors
	void TransformComp::SetPosition(const AEVec3& pos3D)
	{
		mLocal.mTranslation = pos3D;
	}
	void TransformComp::SetScale(const AEVec2& scale)
	{
		mLocal.mScale = scale;  
	}
	void TransformComp::SetOrientation(float rotZ)
	{
		mLocal.mOrientation = rotZ;
	}
	void TransformComp::SetWorldPosition(const AEVec3& pos3D)
	{
		if (mParentTransform)
		{
			mWorld.mTranslation = pos3D;
			mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		}
		else
			mLocal.mTranslation = mWorld.mTranslation = pos3D;
	}
	void TransformComp::SetWorldScale(const AEVec2& scale)
	{
		if (mParentTransform)
		{
			mWorld.mScale = scale;
			mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		}
		else
			mLocal.mScale = mWorld.mScale = scale;
	}
	void TransformComp::SetWorldOrientation(float rotZ)
	{
		if (mParentTransform)
		{
			mWorld.mOrientation = rotZ;
			mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		}
		else
			mLocal.mOrientation = mWorld.mOrientation = rotZ;
	}

	void TransformComp::Translate(const AEVec3& pos3D)
	{
		if (mParentTransform)
		{
			mWorld.mTranslation += pos3D;
			mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		}
		else
			mLocal.mTranslation += pos3D;
	}

	void TransformComp::Scale(const AEVec2& scale)
	{
		AEVec2 sc = { scale.x / mWorld.mScale.x, scale.y / mWorld.mScale.y };
		if (mParentTransform)
		{
			// not sure
			mWorld.mScale.x *= sc.x;
			mWorld.mScale.y *= sc.y;
			mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		}
		else
		{
			// not sure
			mLocal.mScale.x *= sc.x;
			mLocal.mScale.y *= sc.y;
		}
	}

	void TransformComp::Rotate(float rotZ)
	{
		SetOrientation(rotZ);
		//if (mParentTransform)
		//{
		//	mWorld.mOrientation += rotZ;
		//	mLocal = mParentTransform->mWorld.InverseConcat(mWorld);
		//}
		//else
		//	mLocal.mOrientation += rotZ;
	}

	// Gettors
	const AEVec3& TransformComp::GetWorldPosition()
	{
		return mWorld.mTranslation;
	}
	const AEVec2& TransformComp::GetWorldScale()
	{
		return mWorld.mScale;
	}
	float TransformComp::GetWorldOrientation()
	{
		return mWorld.mOrientation;
	}
	const AEVec3& TransformComp::GetPosition()
	{
		return mLocal.mTranslation;
	}
	const AEVec2& TransformComp::GetScale()
	{
		return mLocal.mScale;
	}
	float TransformComp::GetOrientation()
	{
		return mLocal.mOrientation;
	}

	void TransformComp::DebugDraw(Color col, bool inlocal)
	{
		auto gfx = aexGraphics;
		Transform* tr = inlocal ? &mLocal : &mWorld;

		gfx->DrawOrientedRect(tr->mTranslation.x, tr->mTranslation.y, tr->mScale.x, tr->mScale.y, tr->mOrientation, col);
	}

	void TransformComp::StreamRead(const nlohmann::json& j)
	{
		// read only the local transform
		j["local"] >> mLocal;
	}

	void TransformComp::StreamWrite(nlohmann::json& j) const
	{
		// write the local transform
		j["local"] << mLocal;
	}
}