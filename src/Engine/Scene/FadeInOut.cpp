#include "FadeInOut.h"
#include "imgui/imgui.h"
#include <iostream>
namespace AEX
{
	void Fade::AddToSystem()
	{
		aexLogic->AddComp(this);
	}
	void Fade::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}
	void Fade::OnCreate()
	{}
	void Fade::Initialize()
	{
		renderable = mOwner->GetComp<Renderable>();
		renderable->mModulationColor.a = 1;
	}
	void Fade::Update()
	{
		if (fadingIn)
		{
			renderable->mModulationColor.a -= 0.01;
		
			std::cout << renderable->mModulationColor.a << std::endl;
		
			if (renderable->mModulationColor.a <= 0)
				fadingIn = false;
		}

		if (fadingOut)
		{
			renderable->mModulationColor.a += 0.01;
		
			if (renderable->mModulationColor.a >= 255)
				fadingOut = false;
		}
	}
	void Fade::Shutdown()
	{
		RemoveFromSystem();
	}
	bool Fade::Edit()
	{
		return false;
	}
	void Fade::StreamRead(const nlohmann::json& j)
	{
	}
	void Fade::StreamWrite(nlohmann::json& j) const
	{
	}
}