#pragma once
#include <Core/AEXCore.h>
#include <Logic/AEXLogicSystem.h>				// LogicComp
#include <Collisions/AEXCollisionSystem.h>		// Collider, CollisionStayEvent
#include <Graphics/Components/AEXGfxComps.h>	// Renderable, Texture

namespace AEX
{
	class BreakableComp : public LogicComp
	{
		AEX_RTTI_DECL(BreakableComp, LogicComp);

	public:

		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;
		bool Edit() override;

		// logic
		void Break(const CollisionStayEvent&);
		virtual void Act();

		void StreamRead(const nlohmann::json& j);
		void StreamWrite(nlohmann::json& j) const;

	private:
		bool mBroken = false;
		bool* mCol_ghost = nullptr;
		TResource<Texture>* mBrokenTex = nullptr;
		unsigned mReactTag = 0;
	};
}