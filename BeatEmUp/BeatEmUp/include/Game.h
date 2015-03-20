#pragma once
#include <string>
#include "SDLApp.h"
#include "Background.h"
#include "Player.h"
#include "Roamer.h"
#include "Enemy.h"
#include "Text.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define GAME				Game::Instance()



//Game class
//High level game logic
//Nasir 2015.03.01
class Game : public SDLApp, public Singleton<Game>
{
public:
	//area of the screen where objects can move/roam
	const RectF MoveBounds;

	//Game objects
	GameObjectList gameObjects;
	Background* bg;
	TextBlock *tbFps, *tbPlayerPos, *tbEnemyPos;
	Player* player;
	vector<Enemy*> enemies;

private:
	bool leftDown;
	bool rightDown;
	bool upDown;
	bool downDown;


public:
	//Accessors
	__forceinline int MidSectionY(int myHeight) const { return clientHeight_ - myHeight - (int)(MoveBounds.h / 2); }
	__forceinline int MidSectionX(int myWidth) const { return (clientWidth_ / 2) - (myWidth / 2); }
	
	__forceinline int RandomYWithinMoveBounds(int myHeight) const
	{
		const int min = (int)((int)clientHeight_ - (int)MoveBounds.h - myHeight);
		const int max = (int)clientHeight_ - myHeight;
		return WHEEL_OF_FORTUNE.Next(min, max);
	}

public:
	//Overrides
	Game();
	virtual ~Game();
	virtual bool Init() override;
	virtual void ProcessEvent(const SDL_Event& e) override;
	virtual void Update() override;
	virtual void Render() override;


private:
	void Stop();
};
