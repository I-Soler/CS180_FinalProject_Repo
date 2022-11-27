#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"

using namespace AEX;

Model* gpQuad;
Texture* gpTex;
ShaderProgram* gpShader;


void DemoMartaLayers::LoadResources()
{
	// Images
	gpTex = aexGraphics->LoadTexture(".\\data\\Images\\Default.png");

	// Models
	gpQuad = aexGraphics->LoadModel(".\\data\\Models\\Quad.model");

	auto vtxShader = aexGraphics->LoadShader("data/Shaders/vertexTransform.vert");

	auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");

	gpShader = aexGraphics->LoadShaderProgram("Texturemap.shader", fragShader, vtxShader);

}

void CreateDemoScene()
{
	//Create a space
	Space* MainSpace = aexScene->NewSpace("MainMenu");
	aexScene->SetName("MainScene");
	//Add components to it
	GameObject* Player = MainSpace->NewObject("Player");
	TransformComp* tr = Player->NewComp<TransformComp>();

	// ... Create all the other components

	tr->mLocal.mScale = { 300,300 };
	Renderable* ren = Player->NewComp<Renderable>();
	ren->mpModel = gpQuad;
	ren->mpShader = gpShader;
	ren->mpTexture = gpTex;

	Player->OnCreate();
	
	GameObject* Cam = MainSpace->NewObject("Camera");
	tr = Cam->NewComp<TransformComp>();
	tr->mLocal.mTranslation = { 0,0,20 };
	Camera* cam = Cam->NewComp<Camera>();

	cam->mbClearViewport = false;
	cam->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());
	Cam->OnCreate();
	

	//Create a space
	Space* SecondSpace = aexScene->NewSpace("SecondSpace");
	aexScene->SetName("SecondSpace");
	//Add components to it
	GameObject* Player2 = SecondSpace->NewObject("Player2");
	TransformComp* tr2 = Player2->NewComp<TransformComp>();

	// ... Create all the other components

	tr2->mLocal.mScale = { 300,300 };
	tr2->mLocal.mTranslation = { 500, 0, 0 };
	Renderable* ren2 = Player2->NewComp<Renderable>();
	ren2->mpModel = gpQuad;
	ren2->mpShader = gpShader;
	ren2->mpTexture = gpTex;

	Player2->OnCreate();

	GameObject* Cam2 = SecondSpace->NewObject("Camera2");
	tr2 = Cam2->NewComp<TransformComp>();
	tr2->mLocal.mTranslation = { 0,0,20 };
	Camera* cam2 = Cam2->NewComp<Camera>();

	cam2->mbClearViewport = false;
	cam2->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam2->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());
	Cam2->OnCreate();


	aexScene->SaveFile("data/Scenes/DemoMartaLayers.json");
}
void DemoMartaLayers::Initialize()
{
	//aexScene->LoadFile("data/Scenes/DemoMartaLayers.json", true);
	CreateDemoScene();
}
void DemoMartaLayers::Update()
{
	auto tr = aexScene->GetMainSpace()->GetChildren()[0]->GetComp<TransformComp>();
	if (aexInput->KeyPressed('A'))
		tr->mLocal.mTranslation.x--;
	if (aexInput->KeyPressed('S'))
		tr->mLocal.mTranslation.y--;
	if (aexInput->KeyPressed('D'))
		tr->mLocal.mTranslation.x++;
	if (aexInput->KeyPressed('W'))
		tr->mLocal.mTranslation.y++;

	aexScene->SaveFile("data/Scenes/DemoMartaLayers.json");
}
void DemoMartaLayers::Render()
{

	// sanity check -> only draw if we have a quad object created

}