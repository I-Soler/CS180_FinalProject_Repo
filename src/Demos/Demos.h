#pragma once
#include <Logic\AEXGameState.h>
// demo game state
class DemoGameState : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
};
class SpaceDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
};

class EngineDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
	virtual void Shutdown();
};

class DemoEditor : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
};

class DemoMartaLayers : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
};

class DemoColThings : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
};

class EventSysDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void Update();
};

class SerializationDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
};

class FactoryDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
};

class RenderableDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
};

class PrototypeDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
	virtual void Shutdown();
};

class SpineDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
	virtual void Shutdown();
};