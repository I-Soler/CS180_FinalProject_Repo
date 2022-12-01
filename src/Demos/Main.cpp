
#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
#include <AEX.h>
#include <iostream>
#include "Demos.h"



void main()
{
	aexEngine->Initialize();
	aexEngine->Run(new PrototypeDemo);
	//aexEngine->Run(new DemoMarta);
	//aexEngine->Run(new SpineDemo);
	AEX::Singletons::ReleaseAll();
}