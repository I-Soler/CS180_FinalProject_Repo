#include <AEX.h>
#include "Demos.h"
#include "Commons.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Extern/json/json.hpp>
#include <Platform/AEXSerialization.h>
#include "ResourceMgr/ResourceMgr.h"
#include <Graphics/AEXShader.h>
#include <Graphics/AEXTexture.h>

using nlohmann::json;


void GettingStarted() {
	json j;
	j["hello"] = "World";
	j["anInt"] = 123;
	j["aReal"] = 3.1415f;
	j["aBool"] = true;

	json jar;
	{
		json tmp;
		tmp["hello"] = "world";
		jar.push_back(tmp);
	}
	{
		json tmp;
		tmp["Zello"] = "world";
		jar.push_back(tmp);
	}
	{
		json tmp;
		tmp["Aello"] = "world";
		jar.push_back(tmp);
	}

	j["an_array"] = jar;


	std::cout << std::setw(4) << j;

	SaveJsonToFile(j, "data/test.json");
}
// repeat for AEVec3, AEVec4, Color, Transform (tr,sc,rot)

void CreateDummyObject() 
{
	AEX::GameObject* obj = aexFactory->Create<AEX::GameObject>();

	obj->SetName("Thomas");
	// add components to it
	obj->NewComp<AEX::TransformComp>();
	obj->NewComp<AEX::Renderable>();
	obj->NewComp<AEX::Camera>();

	json j;
	obj->StreamWrite(j);

	SaveJsonToFile(j, "data/test.json");
}

void CreateSimpleScene()
{
	auto gpQuad = aexGraphics->LoadModel("quad.model");
	//auto gpTex = aexGraphics->LoadTexture("data/Images/a.jpg");
	//
	//auto vtxShader = aexGraphics->LoadShader("data/Shaders/VertexTransform.vert");
	//auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");
	//auto gpShader = aexGraphics->LoadShaderProgram("TextureMap.shader", fragShader, vtxShader);

	// create a game object 
	AEX::GameObject* obj = new AEX::GameObject;

	// add components to it
	auto tr = obj->NewComp<AEX::TransformComp>();
	tr->SetScale({ 300 , 300 });
	auto ren = obj->NewComp<AEX::Renderable>();
	ren->mpModel = gpQuad;
	//ren->mpShader = gpShader;
	//ren->mpTexture = gpTex;
	//obj->OnCreate();	// call on craete (after ALL comps of the object have been created). 

	AEX::GameObject* obj2 = new AEX::GameObject;
	tr = obj2->NewComp<AEX::TransformComp>();
	tr->SetPosition({ 0,0, 20 });
	auto cam = obj2->NewComp<AEX::Camera>();
	cam->mbClearViewport = true;
	cam->mViewportColor = AEX::Color(1, 0.5, 0);
	cam->mViewport = AEX::AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
	cam->mViewRectangle = AEX::AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());
	obj2->NewComp<CamController>();
	//obj2->OnCreate();

	// call initialize when done (supposed to be done after ALL scene is loaded)
	//obj->Initialize();
	//obj2->Initialize();

	// TODO: move this to the scene singleton. 
	json sceneJson, obj1Json, obj2Json;
	obj->StreamWrite(obj1Json);
	obj2->StreamWrite(obj2Json);
	sceneJson.push_back(obj1Json);
	sceneJson.push_back(obj2Json);
	SaveJsonToFile(sceneJson, "data/simple.json");

}

void SerializationDemo::LoadResources()
{
	aexGraphics->LoadModel("quad.model");
	//aexGraphics->LoadTexture("data/Images/Default.png");
	////
	//auto vtxShader = aexGraphics->LoadShader("data/Shaders/VertexTransform.vert");
	//auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");
	////aexGraphics->LoadShaderProgram("TextureMap.shader", fragShader, vtxShader);

	//resource manager mini demo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	aexResources->Initialize();
	aexResources->LoadResource("data/Images/a.jpg");
	auto res = aexResources->GetResource<AEX::Texture>("Default2.jpg");

	auto frag = aexResources->LoadResource("data/Shaders/TextureMap.shader");

	//aexResources->FreeResource("a.jpg");
	aexResources->Shutdown();
}
std::list<AEX::GameObject*> objects;
void ResetScene()
{
	for (auto obj : objects) {
		obj->Shutdown();
		delete obj;
	}
	objects.clear();
}

void LoadSimpleScene()
{
	json sceneJson;
	ReadJsonFromFile(sceneJson, "data/simple.json");

	for (auto objJson : sceneJson)
	{
		auto obj = aexFactory->Create<AEX::GameObject>();
		obj->StreamRead(objJson);
		obj->OnCreate();
		objects.push_back(obj);
	}
	for (auto obj : objects)
		obj->Initialize();
}

void SerializationDemo::Initialize() {
	//GettingStarted();
	//AEX::AEVec2 mTarget;
	//json j;
	//j["target"] << mTarget;
	//SaveJsonToFile(j, "data/test.json");

	aexFactory->Register<AEX::GameObject>();
	aexFactory->Register<AEX::Camera>();
	aexFactory->Register<AEX::TransformComp>();
	aexFactory->Register<AEX::Renderable>();
	aexFactory->Register<CamController>();

	//CreateDummyObject();
	//CreateSimpleScene();
	LoadSimpleScene();

	// create a game object
	//GameObject* obj = aexFactory->Create<GameObject>();

	//json j; ReadJsonFromFile(j, "data/test.json");
	//
	//obj->StreamRead(j);
	//DebugPrint("Name is %s\n", obj->GetName());

}

void SerializationDemo::Update()
{
	// reload of the json...muhahahahahah
	if (aexInput->KeyTriggered('L')) {
		ResetScene();
		LoadSimpleScene();
	}
}