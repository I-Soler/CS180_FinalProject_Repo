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
#include "StateMachine/AEXStateMachineSystem.h"
#include "StateMachine/PlayerStates.h"
#include "Editor/AEXEditor.h"
#include "Audio/Audio.h"
#include "Audio/AudioSourceComp.h"
#include "ResourceMgr/ResourceMgr.h"
#include "Scene/SceneHandler.h"
#include "Scene/FadeInOut.h"

// Logic
#include "Logic/AEXLogicSystem.h"
#include "GameComponents/PlayerMovement.h"
#include "GameComponents/PlayerAttack.h"
#include "GameComponents/Door.h"
#include "GameComponents/PressurePlate.h"
#include "GameComponents/PressPuzzleController.h"
#include "GameComponents/EnemyHealth.h"
#include "GameComponents/BreakableComp.h"
#include "GameComponents/InfoScreen.h"
#include "GameComponents/Button.h"
#include "GameComponents/BubbleComp.h"


#include "Engine/Graphics/AEXTextureAtlas.h"

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
	};
}


#endif