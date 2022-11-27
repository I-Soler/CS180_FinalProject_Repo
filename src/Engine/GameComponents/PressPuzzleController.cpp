#include "PressPuzzleController.h"
#include <iostream>

namespace AEX
{
	void PressPuzzleController::AddToSystem()
	{
		aexLogic->AddComp(this);
	}

	void PressPuzzleController::RemoveFromSystem()
	{
		aexLogic->RemoveComp(this);
	}

	void PressPuzzleController::OnCreate()
	{
		plates.clear();
	}
	void PressPuzzleController::Initialize()
	{
	}
	void PressPuzzleController::Update()
	{
	}
	void PressPuzzleController::Shutdown()
	{
		RemoveFromSystem();
	}

	//logic for when a PressurePlate is pressed
	bool PressPuzzleController::OnPressurePlatePressed(char PressurePlateID)
	{
		//DrawDebugState();   //debug function

		//check if the PressurePlate is pressed in correct order
		if (puzzlesolution[counter] == PressurePlateID)
		{
			//std::cout << "AREBEK : "<< counter << std::endl;
			++counter;
			if (counter >= puzzlesize)
			{
				EndPuzzle();
			}
			//std::cout << PressurePlateID << std::endl;
			return true;
		}
		else
		{
			//add functionality for the game (less hp for player)
			ResetPuzzle();
			return false;
		}
	}

	//adds all puzzle PressurePlates to the vector
	void PressPuzzleController::AddPressurePlate(PressurePlate* rhs)
	{
		//std::cout << "AREBEK DEBUG ADD PressurePlate TO PUZZLE : " << rhs->id_[0] << std::endl;  //debuggig purposes
		plates.push_back(rhs);
	}

	//if you fail, it resets the puzzle
	void PressPuzzleController::ResetPuzzle()
	{
		//reset PressurePlate state
		for (int i = 0; i < plates.size(); i++)
		{
			plates[i]->ResetPressurePlate();
		}
		counter = 0;
	}

	//if you complete the puzzle
	void PressPuzzleController::EndPuzzle()
	{
		for (int i = 0; i < plates.size(); i++)
		{
			plates[i]->SetEndState();
		}
	}

	//function for debug purposes
	void PressPuzzleController::DrawDebugState()
	{
		std::cout << "AREBEK DEBUG PressurePlate STATES ////////////" << std::endl;
		for (int i = 0; i < plates.size(); i++)
		{
			plates[i]->PrintDebugState();
		}
		std::cout << "AREBEK DEBUG END           ////////////" << std::endl;
	}
}