#include <Core/AEXCore.h>
#include <Composition/TransformComponent.h>
#include "AEXPhysicsSystem.h"
#include "RigidbodyComponent.h"
#include "Platform/AEXTime.h"
#include <Extern/imgui/imgui.h>
#include <Extern/imgui/ImGuizmo.h>

namespace AEX
{
	RigidbodyComp::RigidbodyComp()
	{
		mGravity = 0.0f;
		mInvMass = 1.0f;
		mDrag = 1.0f;
	}
	RigidbodyComp::~RigidbodyComp()
	{}
	void RigidbodyComp::Initialize()
	{
		// Get and set Transform.position
		mObjTr = GetOwner()->GetComp<TransformComp>();
		mPrevPosition = mObjTr->GetPosition();
		mVelocity = AEVec2(0, 0);
		mAcceleration = AEVec2(0, 0);

		AddToSystem();
	}
	bool RigidbodyComp::Edit()
	{
		if (ImGui::CollapsingHeader("Rigid Body"))
		{
			float mass = 1.0f / mInvMass;
			ImGui::InputFloat("Mass", &mass);
			if(ImGui::InputFloat("Gravity", &mGravity))
				mHasGravity = true;
			ImGui::InputFloat("Drag", &mDrag);


			if(mass <= 0)
				mass = 0.01f;

			mInvMass = 1 / mass;
			return true;
		}
		return false;
	}
	void RigidbodyComp::OnCreate()
	{
	}
	void RigidbodyComp::Update()
	{
		// add gravity
		if (mHasGravity && mInvMass)
			AddForce(AEVec2(0.0f, -mGravity) / mInvMass);

		// Integrate physics
		Integrate();
	}
	void RigidbodyComp::Shutdown()
	{
		RemoveFromSystem();
	}
	void RigidbodyComp::AddToSystem()
	{
		aexPhysics->AddRBody(this);
	}
	void RigidbodyComp::RemoveFromSystem()
	{
		aexPhysics->RemoveRBody(this);
	}
	void RigidbodyComp::AddForce(AEVec2 force)
	{
		mAcceleration += force;
	}
	void RigidbodyComp::Integrate()
	{
		if (mInvMass == 0)
			return;

		//f32 dt = aexTime->GetFrameTime();
		f32 dt = 0.016666667f;

		mPrevPosition = mObjTr->GetPosition();
		mAcceleration *= mInvMass;
		mVelocity += mAcceleration * dt;
		mVelocity *= mDrag;		// 0 <= drag <= 1
		mObjTr->SetPosition(mPrevPosition + mVelocity * dt);
		mAcceleration = AEVec2(0, 0);
	}
	void RigidbodyComp::StreamRead(const nlohmann::json& j)
	{
		mGravity = j["Gravity"];
		mInvMass = j["InvMass"];
		mDrag = j["Drag"];
	}
	void RigidbodyComp::StreamWrite(nlohmann::json& j) const
	{
		j["Gravity"] = mGravity;
		j["InvMass"] = mInvMass;
		j["Drag"] = mDrag;
	}
	//void RigidbodyComp::Render(AEMtx33* pCamMtx)
	//{
	//	// render just the aabb
	//	AEGfxReset();
	//	if (pCamMtx)
	//		AEGfxSetTransform(pCamMtx);
	//	switch (mCollisionShape)
	//	{
	//	case CSHAPE_OBB:
	//	case CSHAPE_AABB: {
	//		AEGfxRectRot(mTransform.mPosition.x, mTransform.mPosition.y, 0, mTransform.mScale.x, mTransform.mScale.y, mTransform.mOrientation, mColor);
	//		break;
	//	}
	//	case CSHAPE_CIRCLE: {
	//		AEGfxCircle(mTransform.mPosition.x, mTransform.mPosition.y, mTransform.mScale.x, mColor);
	//		break;
	//	}
	//	}
	//	AEGfxFlush();
	//}
}