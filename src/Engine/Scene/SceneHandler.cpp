#include "SceneHandler.h"
#include <Extern/imgui/imgui.h>
#include "Editor/AEXEditor.h"
#include "FadeInOut.h"
#include "AEX.h"

namespace AEX
{
	void SceneHandler::OnCreate()
	{
	}
	void SceneHandler::Initialize()
	{
		if (ChangesWithTime)
		{
			timer.Start();	
			//GameObject* obj = aexScene->GetMainSpace()->FindObjectByName("Fade");
			//obj->GetComp<Fade>()->fadingIn = true;
		}
	}
	void SceneHandler::Update()
	{	
		if (ChangesWithTime && timer.GetTimeSinceStart() > TimeToFade)
		{
			GameObject* obj = aexScene->GetMainSpace()->FindObjectByName("Fade");
			obj->GetComp<Fade>()->fadingOut = true;
		}

		if (ChangesWithTime && timer.GetTimeSinceStart() > TimeToChange)
		{
			aexEngine->changeScene.HasToChange = true;
			aexEngine->changeScene.newScene = ChangeToScene;
		}
	}

	void SceneHandler::ChangeScene()
	{
		aexEngine->changeScene.HasToChange = true;
		aexEngine->changeScene.newScene = ChangeToScene;
	}

	void SceneHandler::Shutdown()
	{
		RemoveFromSystem();
	}
	bool SceneHandler::Edit()
	{
		static char label[30] = "Empty";		// enter name to save file
		strcpy(label, ChangeToScene.c_str());

		ImGui::Checkbox("Changes with time", &ChangesWithTime);

		if(ChangesWithTime)
			ImGui::InputFloat("Time till changes", &TimeToChange);			
		
		if(ChangesWithTime)
			ImGui::InputFloat("Time till it starts fading", &TimeToFade);

		if (ImGui::InputText("Scene to change to", label, 30, ImGuiInputTextFlags_EnterReturnsTrue))
			ChangeToScene = label;
		
		return true;

	}
	void SceneHandler::StreamRead(const nlohmann::json& j)
	{
		ChangesWithTime = j["ChangesWithTime"];
		TimeToChange = j["TimeToChange"];
		TimeToFade = j["TimeToFade"];
		ChangeToScene = j["ChangeToScene"];
	}
	void SceneHandler::StreamWrite(nlohmann::json& j) const
	{
		j["ChangesWithTime"] = ChangesWithTime;
		j["TimeToChange"] = TimeToChange;
		j["TimeToFade"] = TimeToFade;
		j["ChangeToScene"] = ChangeToScene.c_str();
	}

}