#ifndef AEX_H_
#define AEX_H_

#include "Debug\AEXDebug.h"
#include "Core\AEXCore.h"
#include "Composition\AEXComposition.h"
#include "Platform\AEXPlatform.h"
#include "Logic\AEXGameState.h"
#include "Graphics\AEXGraphics.h"
#include "Collisions\AEXCollisionSystem.h"
#include "Physics/AEXPhysicsSystem.h"
#include "Physics/RigidbodyComponent.h"
#include "Engine/Scene/Scene.h"
#include "Editor/AEXEditor.h"
#include "ResourceMgr/ResourceMgr.h"


// Logic
#include "Logic/AEXLogicSystem.h"
#include "GameComponents/BubbleComp.h"
#include "GameComponents/Bullet.h"
#include "GameComponents/Turret.h"


// Easy access to singleton
#define aexEngine (AEX::Singletons::Get<AEX::MainEngine>())

namespace AEX{

	struct ChangeScene
	{
		bool HasToChange;
		std::string newScene;
	};

	class MainEngine : public IBase
	{
		AEX_RTTI_DECL(MainEngine, IBase);
		AEX_SINGLETON(MainEngine);
	public:
		virtual bool Initialize();
		void Run(IGameState*gameState = nullptr);

		ChangeScene changeScene;
		bool Multithreaded = true;
	};
}


#endif