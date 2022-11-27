#include <AEX.h>
#include "Demos.h"
#include <iostream>

using namespace AEX;


class LogicComp : public IComp
{
	void AddToSystem() override;
	void RemoveFromSystem() override;
};

class Logic : public IBase
{
	AEX_RTTI_DECL(Logic, IBase);
	AEX_SINGLETON(Logic)
	{}

	std::vector<LogicComp*> mComps;
public:
	void AddComp(LogicComp* l) {}
	void RemoveComp(LogicComp* l){}
	void ClearAllComps() {}
	void Update()
	{
		for (auto comp : mComps)
			if(comp->Enabled())
				comp->Update();
	}
};


void LogicComp::AddToSystem()
{
	Singletons::Get<Logic>()->AddComp(this);
}
void LogicComp::RemoveFromSystem()
{
	Singletons::Get<Logic>()->RemoveComp(this);
}

class CompA : public IComp
{
	AEX_RTTI_DECL(CompA, IComp);

public: 
	int MyInt;

	void OnCreate() { DebugPrint("CompA: OnCreate\n"); }
	void Initialize() { DebugPrint("CompA: Initialize\n"); }
	void Shutdown() { DebugPrint("CompA: Shutdown\n"); }
	void Update() { DebugPrint("CompA: Update\n"); }
};

void TestComposition()
{
	{
		GameObject obj;
		//obj.AddComp(new CompA)->MyInt;
		// CompA* pA = dynamic_cast<CompA*>(obj.GetComp("compA"));
		obj.NewComp<CompA>()->MyInt;
		CompA* pA = obj.GetComp<CompA>();
	}
	{
		CompA a;
		DebugPrint("%s, (parent is: %s)\n", CompA::TYPE().GetName().c_str(),
			CompA::TYPE().GetParentTypeName().c_str());

		IComp* pComp = &a;
		std::string type = pComp->GetType().GetName().c_str();
	}
}

// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void CompositionDemo::Initialize()
{
	TestComposition();

	GameObject* go = new GameObject;
	auto logic = go->NewComp<LogicComp>();

}
void CompositionDemo::LoadResources()
{

}
void CompositionDemo::Update()
{

}
void CompositionDemo::Render()
{
	auto mainWin = aexWindow;
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	aexGraphics->SetViewport(0, 0, s32(winWH.x), s32(winWH.y));
	aexGraphics->SetClearColor(Color(0.7f,0.7f,0.7f,1.0f));
	aexGraphics->ClearFrameBuffer();

	aexGraphics->Present();
}