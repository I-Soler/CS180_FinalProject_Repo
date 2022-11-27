#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/PlayerStates.h"

using namespace AEX;

Model* gpQuad = nullptr;
Texture* gpTex = nullptr;
ShaderProgram* gpShader = nullptr;

GameObject* obj = new GameObject;

void StateMachineDemo::LoadResources()
{
	// Images
	gpTex = aexGraphics->LoadTexture(".\\data\\Images\\Default.png");

	// Models
	gpQuad = aexGraphics->LoadModel(".\\data\\Models\\Quad.model");

	auto vtxShader = aexGraphics->LoadShader("data/Shaders/vertexTransform.vert");

	auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");

	gpShader = aexGraphics->LoadShaderProgram("Texturemap.shader", fragShader, vtxShader);

}

void StateMachineDemo::Initialize()
{
	//Add components to it
	auto tr = obj->NewComp<TransformComp>();
	tr->mLocal.mScale = { 300,300 };
	auto ren = obj->NewComp<Renderable>();
	ren->mpModel = gpQuad;
	ren->mpShader = gpShader;
	ren->mpTexture = gpTex;

	/*..............................................................*/
	SuperStateComp* ssc = obj->NewComp<SuperStateComp>();
	ssc->AddState<PlayerIdleState>("PlayerIdleState");
	ssc->AddState<PlayerHorizontalState>("PlayerHorizontalState");
	ssc->AddState<PlayerVerticalState>("PlayerVerticalState");
	/*..............................................................*/
	obj->OnCreate();

	GameObject* obj2 = new GameObject;
	tr = obj2->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { 0,0,20 };
	auto cam = obj2->NewComp<Camera>();
	cam->mbClearViewport = false;
	cam->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());
	obj2->OnCreate();

	obj->Initialize();
	obj2->Initialize();
}
void StateMachineDemo::Update()
{
	//auto tr = obj->GetComp<TransformComp>();
	//if (aexInput->KeyPressed('A'))
	//	tr->mLocal.mTranslation.x--;
	//if (aexInput->KeyPressed('S'))
	//	tr->mLocal.mTranslation.y--;
	//if (aexInput->KeyPressed('D'))
	//	tr->mLocal.mTranslation.x++;
	//if (aexInput->KeyPressed('W'))
	//	tr->mLocal.mTranslation.y++;
}
void StateMachineDemo::Render()
{

}