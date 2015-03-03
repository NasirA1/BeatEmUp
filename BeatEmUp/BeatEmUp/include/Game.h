#pragma once
#include <string>
#include "SDLApp.h"
#include "Background.h"
#include "Player.h"
#include "Rock.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


//Game class
//High level game logic
//Nasir 2015.03.01
class Game : public SDLApp
{
public:
	GameObjectList gameObjects;
	Background* bg;
	Player* player;
	Rock* rock;
	Knight* knight;

private:
	bool leftDown;
	bool rightDown;
	bool upDown;
	bool downDown;
	RectF moveBounds;


public:
	//Overrides
	Game();
	virtual ~Game();
	virtual bool Init() override;
	virtual void ProcessEvent(const SDL_Event& e) override;
	virtual void Update() override;
	virtual void Render() override;


	//Accessors
	__forceinline const RectF& MoveBounds() const { return moveBounds; }


private:
	void Stop();
};
