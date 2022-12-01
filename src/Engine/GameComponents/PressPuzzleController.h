#pragma once

#include "..\Core\AEXCore.h"
#include"Logic/AEXLogicSystem.h"
#include "PressurePlate.h"
#include <vector>


namespace AEX
{
	class PressurePlate;

	class PressPuzzleController : public LogicComp
	{
		AEX_RTTI_DECL(PressPuzzleController, LogicComp);

	public:
		//functions to make it a component selectable in the editor
		void AddToSystem() override;
		void RemoveFromSystem() override;
		void OnCreate() override;
		void Initialize() override;
		void Update() override;
		void Shutdown() override;

		//logic for when a PressurePlate is pressed
		bool OnPressurePlatePressed(char PressurePlateID);
		//adds all puzzle PressurePlates to the vector
		void AddPressurePlate(PressurePlate* rhs);

		//vector with all the PressurePlates that the puzzle will have
		std::vector<PressurePlate*> plates;
	private:
		int puzzlesize = plates.size();
		char puzzlesolution[5] = { '1', '2', '3', '4', '5' };
		int counter = 0;

		//if you fail, it resets the puzzle
		void ResetPuzzle();
		//if you complete the puzzle 
		void EndPuzzle();
		//function for debug purposes
		void DrawDebugState();

	};	
}
