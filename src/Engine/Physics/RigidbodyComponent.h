#pragma once
#include <Core/AEXCore.h>
#include <aexmath/AEXMath.h>
#include <Composition/AEXComponent.h>

namespace AEX
{
	class TransformComp;
	class RigidbodyComp : public IComp
	{
		AEX_RTTI_DECL(RigidbodyComp, IComp);

		public:

		RigidbodyComp();
		~RigidbodyComp();
		// update
		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown()override;

		void AddToSystem() override;
		void RemoveFromSystem() override;

		bool Edit() override;

		// physics data
		AEVec3  mPrevPosition;
		AEVec3	mVelocity;
		AEVec3	mAcceleration;
		f32		mGravity;
		f32		mInvMass;
		f32		mDrag;

		// true if has collided this frame
		bool	mbHasCollided;
		bool	mHasGravity;
		TransformComp* mObjTr;	// has-a

		// integrate
		void Integrate();

		// adding force
		void AddForce(AEVec2 force);

		// Serialization
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j)const override;

		// render (Provided)
		// virtual void Render(AEMtx33* pCamMtx = NULL);
	};
}
