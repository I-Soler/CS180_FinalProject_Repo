#include <spine/spine.h>
#include <AEX.h>
#include <Animation/SpineComps.h>
#include <Demos/Demos.h>
#include <stdio.h>

using namespace AEX;
TResource<SpineData> *data;

void SpineDemo::LoadResources() {
	auto spData = new SpineData();
	spData->Load("data/Spine/spineboy/spineboy-pro.json", "data/Spine/spineboy/spineboy-pro.atlas");
	data = new TResource<SpineData>;
	data->SetRawResource(spData, false);
}
void SpineDemo::Initialize() {
	//create a new object
	GameObject* obj = aexScene->NewObject("SpineObject");

	TransformComp* tr = aexFactory->Create<TransformComp>();
	obj->AddComp(tr);

	SkeletonAnimComp* skelComp = aexFactory->Create<SkeletonAnimComp>();
	skelComp->mSpineData = data;
	obj->AddComp(skelComp);

	SkeletonRenderable* skelRen = aexFactory->Create<SkeletonRenderable>();
	obj->AddComp(skelRen);
	// fake serialization
	
	obj->OnCreate();
	obj->Initialize();


}
void SpineDemo::Update() {}
void SpineDemo::Render() {}
void SpineDemo::Shutdown() {}