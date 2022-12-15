
#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
#include <AEX.h>
#include <iostream>
#include "Demos.h"



void main()
{
	std::cout << "Enable multithreaded assets loader? \n";
	std::cout << "1-Yes, 0-No \n";
	std::cin >> aexEngine->Multithreaded;
	aexEngine->Initialize();
	aexEngine->Run(new PrototypeDemo);
	//aexEngine->Run(new DemoMarta);
	//aexEngine->Run(new SpineDemo);
	AEX::Singletons::ReleaseAll();
}