#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStates.h"
#include "GameComponents/PlayerMovement.h"
#include "Logic/DetachSoul.h"
#include "Audio/AudioSourceComp.h"

using namespace AEX;

Model* gpQuad = nullptr;
Texture* gpTex = nullptr;
Texture* gpTex2 = nullptr;
Texture* gpTex3 = nullptr;
Texture* gpTex4 = nullptr;
Texture* gpTex5 = nullptr;
Texture* gpTex6 = nullptr;
ShaderProgram* gpShader = nullptr;

GameObject* player = nullptr;
GameObject* soul = nullptr;
GameObject* rock = nullptr;
GameObject* door = nullptr;
GameObject* button = nullptr;
GameObject* player_camera = nullptr;

void PrototypeDemo::LoadResources()
{
	// Audio
	//aexResources->RegisterImporter<SoundImporter>(".mp3");
	//aexResources->LoadResource("data/Audio/Hectorplasma.mp3", false);
	//aexResources->LoadResource("data/Audio/EasyVictory.mp3", false);
}

void PrototypeDemo::Initialize()
{

#pragma region// Add player, soul and camera

	// Scene created with default Space
	aexScene->SetName("main_scene");
	aexScene->InitializeSpaces();

#pragma endregion

	//aexScene->LoadFile("SceneFile", true);
}
void PrototypeDemo::Update()
{
	aexScene->Update();
}
void PrototypeDemo::Render()
{

	// sanity check -> only draw if we have a quad object created

}
void PrototypeDemo::Shutdown()
{
	//aexScene->SaveFile("SceneFile");
	aexScene->Shutdown();	// must implement Space destructor, I guess
}