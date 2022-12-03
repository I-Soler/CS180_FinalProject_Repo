#include "AEX.h"
static bool EditorActive = true;
#include "ResourceMgr/ResourceMgr.h"
namespace AEX{
	MainEngine::MainEngine(){}
	bool MainEngine::Initialize()
	{
		// note, here'were creating and initializing at the same
		// time. by typing the maccro, we're creating the singleton
		// pointer which is returned, we then call initialize on it.
		aexFactory->RegisterAllTypes();
		aexResources->Initialize();
		if (!aexWindow->Initialize())return false;
		if (!aexInput->Initialize())return false;
		if (!aexTime->Initialize())return false;
		if (!aexGraphics->Initialize())return false;
		if (!aexCollision->Initialize())return false;
		if (!aexPhysics->Initialize())return false;
		if (!aexScene->Initialize())return false;
		if (!aexLogic->Initialize())return false;
		if (!aexAudio->Initialize())return false;
		if (!aexGui->Initialize())return false;
		if (!aexEditor->Initialize())return false;

		// Frame rate controller options.
		aexTime->LockFrameRate(true);
		aexTime->SetMaxFrameRate(60.0);
		aexGraphics->SetVSyncEnabled(true);

		auto resMgr = aexResources;
		resMgr->LoadFolder("data/Models", false);
		resMgr->LoadFolder("data/Shaders", false);
		resMgr->LoadFolder("data/Images", true);
		resMgr->LoadFolder("data/Spine/Background", false);
		resMgr->LoadFolder("data/Spine/spineboy", false);
		resMgr->LoadFolder("data/Audio", false);
		resMgr->LoadFolder("data/Prefabs", true);
		resMgr->LoadFolder("EditorData", true);

		changeScene.HasToChange = false;
		changeScene.newScene = " ";

		// all good -> return true
		return true;
	}
	void MainEngine::Run(IGameState *gameState)
	{
		// Sanity check
		if (!gameState)
			return;

		// load game state resources
		gameState->LoadResources();

		// initialize game state
		gameState->Initialize();

		// Reset timer
		aexTime->Reset();

		auto logic	= aexLogic;
		auto scene	= aexScene;
		auto window = aexWindow;
		auto time	= aexTime;
		auto input = aexInput;
		auto gui = aexGui;
		auto editor = aexEditor;
		auto statemachine = aexStateMachine;
		auto phx = aexPhysics;
		auto gfx = aexGraphics;
		auto collisions = aexCollision;
		auto audio = aexAudio;

		// run the game loop
		while (input->KeyTriggered(Keys::Escape) == false &&
			window->Exists())
		{
			time->StartFrame();
			window->Update();		// Process OS messages and respond to window events.
			input->Update();			// Process Input specific messages. 
			gui->NewFrame();

			// clean up previous frame dead objects
			scene->DeleteAllSpaceDeadObjects();

			editor->Update();

			if(changeScene.HasToChange)
			{
				changeScene.HasToChange = false;
				aexScene->LoadFile(AddScenePath(changeScene.newScene).c_str());
			}

			if (!editor->Enabled())
			{
				// update game logic
				logic->Update();
				statemachine->Update();

				// physics system update
				phx->Update();
				scene->UpdateAllSpaceHierarchies();

				// collision system update
				collisions->Update();
				scene->UpdateAllSpaceHierarchies(); // Optimization (update the hierarchies of the collided objects only).
				
				// poor man's reset
				//if (input->KeyPressed('R'))		// NOT WORKING, CHECK LATER
				//{
				//	gameState->Shutdown();
				//	gameState->Initialize();
				//}
			}
			else
			{
				scene->UpdateAllSpaceHierarchies();
			}

			// graphics 
			gfx->Render();		// basic rendering  (note this should be removed in a pure component based engine).
			gui->EndFrame();
			gfx->Present();

			// audio
			audio->Update();

			time->EndFrame();

			// poor man's reset
			//if (aexInput->KeyPressed('R'))		// NOT WORKING, CHECK LATER
			//{
			//	gameState->Shutdown();
			//	gameState->Initialize();
			//}
		}


		gameState->Shutdown();

		// unload resources
		gameState->FreeResources();

		// delete gameState
		delete gameState;
	}
}
