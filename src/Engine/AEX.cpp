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
		if (!aexGui->Initialize())return false;
		//if (!aexEditor->Initialize())return false;

		// Frame rate controller options.
		aexTime->LockFrameRate(true);
		aexTime->SetMaxFrameRate(60.0);
		aexGraphics->SetVSyncEnabled(true);

		auto resMgr = aexResources;
		resMgr->LoadFolder("data/Models", false);
		resMgr->LoadFolder("data/Shaders", false);
		resMgr->LoadFolder("EditorData", true);
		resMgr->LoadFolder("data/Images", true);


		//resMgr->LoadFolder("data/SuperHeavy", false);
		resMgr->LoadFolderMultithreaded("data/SuperHeavy");

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
		auto phx = aexPhysics;
		auto gfx = aexGraphics;
		auto collisions = aexCollision;
		
		aexEditor->Initialize();
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

			//editor->SetEnabled(false);
			editor->Update();

			if (!editor->Enabled())
			{
				// update game logic
				logic->Update();

				// physics system update
				phx->Update();
				scene->UpdateAllSpaceHierarchies();

				// collision system update
				collisions->Update();
				scene->UpdateAllSpaceHierarchies(); // Optimization (update the hierarchies of the collided objects only).
			}
			else
			{
				scene->UpdateAllSpaceHierarchies();
			}

			// graphics 
			gfx->Render();		// basic rendering  (note this should be removed in a pure component based engine).
			gui->EndFrame();
			gfx->Present();


			time->EndFrame();
		}

		gameState->Shutdown();

		// unload resources
		gameState->FreeResources();

		// delete gameState
		delete gameState;
	}
}
