#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStates.h"
#include "Logic/PlayerMovement.h"
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

void EngineDemo::LoadResources()
{
	//// Images
	//gpTex = aexGraphics->LoadTexture(".\\data\\Images\\Default.png");
	//gpTex2 = aexGraphics->LoadTexture(".\\data\\Images\\Rock.jpg");
	//gpTex3 = aexGraphics->LoadTexture(".\\data\\Images\\Background.jpg");
	//gpTex4 = aexGraphics->LoadTexture(".\\data\\Images\\player.png");
	//gpTex5 = aexGraphics->LoadTexture(".\\data\\Images\\ghost.png");
	//gpTex6 = aexGraphics->LoadTexture(".\\data\\Images\\button.png");
	//
	//// Models
	//gpQuad = aexGraphics->LoadModel(".\\data\\Models\\Quad.model");
	//auto vtxShader = aexGraphics->LoadShader("data/Shaders/vertexTransform.vert");
	//auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");
	//gpShader = aexGraphics->LoadShaderProgram("Texturemap.shader", fragShader, vtxShader);

	// Audio
	aexResources->RegisterImporter<SoundImporter>(".mp3");
	aexResources->LoadResource("data/Audio/Hectorplasma.mp3");
	aexResources->LoadResource("data/Audio/EasyVictory.mp3");

	//aexAudio->CreateSound("data/Audio/Hectorplasma.mp3");
	//aexAudio->CreateSound("data/Audio/EasyVictory.mp3");
}

void EngineDemo::Initialize()
{
	
	#pragma region// Add player, soul and camera

	// Scene created with default Space
	aexScene->SetName("main_scene");

	Space* BackgroundSpace = aexScene->GetMainSpace();
	//Add components to it
	//GameObject* Background = BackgroundSpace->NewObject("Background");
	//TransformComp* tr_background = Background->NewComp<TransformComp>();

	// ... Create all the other components

	//tr_background->mLocal.mScale = { 1920, 1080 };
	//tr_background->mLocal.mTranslation = AEVec2{ 0, 0 };
	//Renderable* ren_background = Background->NewComp<Renderable>();
	//ren_background->mpModel = gpQuad;
	//ren_background->mpShader = gpShader;
	//ren_background->mpTexture = gpTex3;
	//Background->OnCreate();

	// Camera
	player_camera = BackgroundSpace->NewObject("CameraBackground");
	TransformComp* Cam_transform = player_camera->NewComp<TransformComp>();
	Cam_transform->mLocal.mTranslation = { 0,0,800 };
	Camera* cam_background = player_camera->NewComp<Camera>();
	cam_background->mbClearViewport = false;
	cam_background->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam_background->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());
	cam_background->OnCreate();

	// New space
	Space* PlayerSpace = aexScene->NewSpace("PlayerSpace");
	
	// create Player and add components
	player = PlayerSpace->NewObject("Player");
	auto tr = player->NewComp<TransformComp>();
	tr->mLocal.mScale = { 120,200 };
	//auto ren = player->NewComp<Renderable>();
	//ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex4;
	auto sm = player->NewComp<SuperStateComp>();
	sm->AddState<PlayerIdleState>("PlayerIdleState");
	sm->AddState<PlayerVerticalUpState>("PlayerVerticalUpState");
	sm->AddState<PlayerVerticalDownState>("PlayerVerticalDownState");
	sm->AddState<PlayerHorizontalState>("PlayerHorizontalState");
	player->NewComp<PlayerMovement>();
	player->NewComp<DetachSoul>();
	auto rig = player->NewComp<RigidbodyComp>();
	auto colPl = player->NewComp<Collider>();
	colPl->mColliderType = Collider::ColliderType::CT_AABB;
	colPl->mRigidBody = rig;
	colPl->Ghost = false;
	colPl->mLocal.mTranslation = AEVec3(0, -0.3, 0);
	colPl->mLocal.mScale = AEVec2(1, 0.4);


	// AUDIO
	auto as = player->NewComp<AudioSourceComp>();
	TResource<Sound>* soundRes = aexResources->GetResource<Sound>("Hectorplasma.mp3");
	as->SetSound(soundRes);
	as->SetGroupId(AudioSourceComp::BACKGROUND);
	as->SetLoop(false);
	as->SetPause(false);
	as->SetVolume(1.0f);
	as->Play();
	//Sound* hectorPlasma = as->SetSound("data/Audio/Hectorplasma.mp3");
	//as->Play(ev);



	
	// create Soul and add components
	soul = PlayerSpace->NewObject("Soul");
	tr = soul->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { -100, 0, 0 };
	tr->mLocal.mScale = { 120, 200 };
	//ren = soul->NewComp<Renderable>();
	//ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex5;
	sm = soul->NewComp<SuperStateComp>();
	sm->AddState<PlayerIdleState>("PlayerIdleState");
	sm->AddState<PlayerVerticalUpState>("PlayerVerticalUpState");
	sm->AddState<PlayerVerticalDownState>("PlayerVerticalDownState");
	sm->AddState<PlayerHorizontalState>("PlayerHorizontalState");
	// auto pm = player->NewComp<PlayerMovement>();		// ommited because it will be added in runtime
	soul->SetEnabled(false);



	AEVec3 vec[3] = { AEVec3(-1920 /2,100,0),
					  AEVec3(1920 /2,-100,0),
					  AEVec3(0,-540,0), };

	AEVec2 sca[3] = {AEVec2(100,1080),
				     AEVec2(100,1080),
                     AEVec2(1920,100), };

	// Create walls
	for (int it = 0; it < 3; it++)
	{
		// create rock and add components
		rock = PlayerSpace->NewObject("rock");
		tr = rock->NewComp<TransformComp>();
		tr->mLocal.mTranslation = vec[it];
		tr->mLocal.mScale = sca[it];
		auto col = rock->NewComp<Collider>();
		col->mColliderType = Collider::ColliderType::CT_AABB;
		col->Ghost = false;
		col->Static = true;
	}

	// Wall arriba izquierda
	rock = PlayerSpace->NewObject("rock");
	tr = rock->NewComp<TransformComp>();
	tr->mLocal.mTranslation = AEVec3(-950, 100, 0);
	tr->mLocal.mScale = AEVec2(1920, 100);
	auto col1 = rock->NewComp<Collider>();
	col1->mColliderType = Collider::ColliderType::CT_AABB;
	col1->Ghost = false;
	col1->Static = true;

	// Wall arriba derecah
	rock = PlayerSpace->NewObject("rock");
	tr = rock->NewComp<TransformComp>();
	tr->mLocal.mTranslation = AEVec3(600, 100, 0);
	tr->mLocal.mScale = AEVec2(500, 100);
	auto col2 = rock->NewComp<Collider>();
	col2->mColliderType = Collider::ColliderType::CT_AABB;
	col2->Ghost = false;
	col2->Static = true;

	// Etxea
	rock = PlayerSpace->NewObject("rock");
	tr = rock->NewComp<TransformComp>();
	tr->mLocal.mTranslation = AEVec3(-350, -50, 0);
	tr->mLocal.mScale = AEVec2(500, 200);
	auto col3 = rock->NewComp<Collider>();
	col3->mColliderType = Collider::ColliderType::CT_AABB;
	col3->Ghost = false;
	col3->Static = true;

	// create door and add components
	//door = PlayerSpace->NewObject("door");
	//tr = door->NewComp<TransformComp>();
	//tr->mLocal.mTranslation = AEVec3(150, 200, 0);
	//tr->mLocal.mScale = AEVec2(400, 400);
	//ren = door->NewComp<Renderable>();
	//ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex2;

	// create rock and add components
	//rock = PlayerSpace->NewObject("rock");
	//tr = rock->NewComp<TransformComp>();
	//tr->mLocal.mTranslation = { 100,-100,0};
	//tr->mLocal.mScale = { 100,100 };
	//ren = rock->NewComp<Renderable>();
	//ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex2;
	//rig = rock->NewComp<RigidbodyComp>();
	//auto col4 = rock->NewComp<Collider>();
	//col4->mColliderType = Collider::ColliderType::CT_AABB;
	//col4->mRigidBody = rig;
	//col4->Ghost = false;
	//col4->mLocal.mTranslation = AEVec3(0, -0.3, 0);
	//col4->mLocal.mScale = AEVec2(1, 0.4);
	//col4->mWorld.mTranslation = { 100, 0, 0 };

	//create button and add components
	//button = PlayerSpace->NewObject("button");
	//tr = button->NewComp<TransformComp>();
	//tr->mLocal.mTranslation = { 400, -100, 0 };
	//tr->mLocal.mScale = { 100,100 };
	//ren = button->NewComp<Renderable>();
	//ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex6;
	//rig = button->NewComp<RigidbodyComp>();
	//auto bcol = button->NewComp<Collider>();
	//bcol->mColliderType = Collider::ColliderType::CT_AABB;
	//bcol->mRigidBody = rig;
	//bcol->Ghost = true;
	//bcol->mWorld.mTranslation = { 400, -100, 0 };



	aexScene->InitializeSpaces();

	#pragma endregion

	//aexScene->LoadFile("SceneFile", true);	// crashes, when entering InitializeSpaces mSpaces is ???
}
void EngineDemo::Update()
{
	//ContactInfo d;
	//
	//if (AABBvsAABB(button->GetComp<Collider>(), rock->GetComp<Collider>(), &d))
	//{
	//	door->GetComp<Renderable>()->mpTexture = NULL;
	//	door->GetComp<Renderable>()->mpShader = NULL;
	//	door->GetComp<Renderable>()->mpModel = NULL;
	//}
	//
	//TransformComp* player_transform = player->GetComp<TransformComp>();
	//AEVec3 pos(player_transform->GetPosition().x, player_transform->GetPosition().y, 800);
	//player_camera->GetComp<TransformComp>()->SetPosition(pos);
	aexScene->Update();
}
void EngineDemo::Render()
{

	// sanity check -> only draw if we have a quad object created

}
void EngineDemo::Shutdown()
{
	//aexScene->SaveFile("SceneFile");
	aexScene->Shutdown();	// must implement Space destructor, I guess
}