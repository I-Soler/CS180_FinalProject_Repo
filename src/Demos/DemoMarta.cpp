#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStates.h"
#include "Logic/PlayerMovement.h"
#include "Logic/DetachSoul.h"

using namespace AEX;

Model* gpQuad = nullptr;
Texture* gpTex = nullptr;
Texture* gpTex2 = nullptr;
ShaderProgram* gpShader = nullptr;

GameObject* player = nullptr;
GameObject* soul = nullptr;
GameObject* rock = nullptr;

void DemoMarta::LoadResources()
{
	// Images
	gpTex = aexGraphics->LoadTexture(".\\data\\Images\\Default.png");
	gpTex2 = aexGraphics->LoadTexture(".\\data\\Images\\Rock.jpg");

	// Models
	gpQuad = aexGraphics->LoadModel(".\\data\\Models\\Quad.model");

	auto vtxShader = aexGraphics->LoadShader("data/Shaders/vertexTransform.vert");

	auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");

	gpShader = aexGraphics->LoadShaderProgram("Texturemap.shader", fragShader, vtxShader);

}

void DemoMarta::Initialize()
{
	
	#pragma region// Add player, soul and camera

	// Scene created with default Space
	aexScene->SetName("Main Scene");
	Space* mainSpace = aexScene->GetMainSpace();	// assign main Space
	
	// create Player and add components
	player = mainSpace->NewObject("Player");
	auto tr = player->NewComp<TransformComp>();
	tr->mLocal.mScale = { 300,300 };
	auto ren = player->NewComp<Renderable>();
	ren->mpModel = gpQuad;
	ren->mpShader = gpShader;
	ren->mpTexture = gpTex;
	auto sm = player->NewComp<SuperStateComp>();
	sm->AddState<PlayerIdleState>("PlayerIdleState");
	sm->AddState<PlayerVerticalUpState>("PlayerVerticalUpState");
	sm->AddState<PlayerVerticalDownState>("PlayerVerticalDownState");
	sm->AddState<PlayerHorizontalState>("PlayerHorizontalState");
	player->NewComp<PlayerMovement>();
	player->NewComp<DetachSoul>();
	auto rig = player->NewComp<RigidbodyComp>();
	auto col = player->NewComp<Collider>();
	col->mColliderType = Collider::ColliderType::CT_AABB;
	col->mRigidBody = rig;
	col->Ghost = false;
	
	// create Soul and add components
	soul = mainSpace->NewObject("Soul");
	tr = soul->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { 100, 0, 0 };
	tr->mLocal.mScale = { 100,100 };
	ren = soul->NewComp<Renderable>();
	ren->mpModel = gpQuad;
	ren->mpShader = gpShader;
	ren->mpTexture = gpTex;
	sm = soul->NewComp<SuperStateComp>();
	sm->AddState<PlayerIdleState>("PlayerIdleState");
	sm->AddState<PlayerVerticalUpState>("PlayerVerticalUpState");
	sm->AddState<PlayerVerticalDownState>("PlayerVerticalDownState");
	sm->AddState<PlayerHorizontalState>("PlayerHorizontalState");
	// auto pm = player->NewComp<PlayerMovement>();		// ommited because it will be added in runtime


	// create rock and add components
	rock = mainSpace->NewObject("rock");
	tr = rock->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { 200, 100, 0 };
	tr->mLocal.mScale = { 100,100 };
	ren = rock->NewComp<Renderable>();
	ren->mpModel = gpQuad;
	ren->mpShader = gpShader;
	ren->mpTexture = gpTex2;
	rig = rock->NewComp<RigidbodyComp>();
	col = rock->NewComp<Collider>();
	col->mColliderType = Collider::ColliderType::CT_AABB;
	col->mRigidBody = rig;
	col->Ghost = false;

	// create Camera and add components
	GameObject* camera = mainSpace->NewObject("Camera");
	tr = camera->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { 0,0,200 };
	auto cam = camera->NewComp<Camera>();
	cam->mbClearViewport = false;
	cam->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());

	aexScene->InitializeSpaces();

	#pragma endregion

	//aexScene->LoadFile("SceneFile", true);	// crashes, when entering InitializeSpaces mSpaces is ???
}
void DemoMarta::Update()
{
	aexScene->Update();
}
void DemoMarta::Render()
{

	// sanity check -> only draw if we have a quad object created

}
void DemoMarta::Shutdown()
{
	//aexScene->SaveFile("SceneFile");
	aexScene->Shutdown();	// must implement Space destructor, I guess
}