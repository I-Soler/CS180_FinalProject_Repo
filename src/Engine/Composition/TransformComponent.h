#pragma once
#include <aexmath/AEXMath.h>
#include <Composition/AEXComponent.h>
#include <Graphics/AEXColor.h>

namespace AEX
{
	class TransformComp : public IComp
	{
		AEX_RTTI_DECL(TransformComp, IComp);

	public:
		TransformComp();
		~TransformComp();

		// fetches any object parent's transform
		void AddToSystem()override;
		void RemoveFromSystem()override;
		void OnCreate() override;
		void Update() override;
		bool Edit() override;
		void Shutdown() override;
		void AddChildTransform(TransformComp* tr);
		void RemoveChildTransform(TransformComp* tr);

		// inlines
		// Changes the local transform directly
		void SetPosition(const AEVec3& pos3D);
		void SetScale(const AEVec2& scale);
		void SetOrientation(float rotZ);

		// Changes the local transform based on the specified world transform
		void SetWorldPosition(const AEVec3& pos3D);
		void SetWorldScale(const AEVec2& scale);
		void SetWorldOrientation(float rotZ);

		// transform globaly
		void Translate(const AEVec3& pos3D);
		void Scale(const AEVec2& scale);
		void Rotate(float rotZ);

		// Get transform variables for Rigidbody
		const AEVec3& GetWorldPosition();
		const AEVec2& GetWorldScale();
		float GetWorldOrientation();

		const AEVec3& GetPosition();
		const AEVec2& GetScale();
		float GetOrientation();

		// Get transform
		const Transform& World() const { return mWorld; }
		TransformComp* Parent() const { return mParentTransform; }

		// Debug Draw in world or local. Default is world
		void DebugDraw(Color col = Color(0, 0, 0, 0), bool inlocal = false);

		// For when we have json done
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;

	public:
		Transform mLocal;
		Transform mWorld;
		std::list<TransformComp*>	mChildren;
	protected:

		// transform hierarchy
		TransformComp*				mParentTransform = nullptr;
	};
}
