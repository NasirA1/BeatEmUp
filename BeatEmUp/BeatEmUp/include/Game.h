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
	Game() 
		: SDLApp(SCREEN_WIDTH
		, SCREEN_HEIGHT
		, "Nasir's Beat 'em Up Game")
		, bg(NULL)
		, player(NULL)
		, leftDown(false)
		, rightDown(false)
		, upDown(false)
		, downDown(false)
	{
		moveBounds.x = 0, moveBounds.y = 370;
		moveBounds.w = SCREEN_WIDTH, moveBounds.h = 120;
	}

	virtual ~Game()
	{
		logPrintf("Game object released");
	}

	virtual bool Init() override
	{
		if(!SDLApp::Init())
			return false;

		bg = new Background(clientWidth_, clientHeight_, renderer_);
		player = new Player(renderer_);
		rock = new Rock("resources/rock.png", renderer_);
		knight = new Knight(renderer_);

		gameObjects.push_back(bg);
		gameObjects.push_back(player);
		gameObjects.push_back(rock);
		gameObjects.push_back(knight);

		return true;
	}

	virtual void ProcessEvent(const SDL_Event& e) override
	{
		if(e.key.state == SDL_PRESSED && !e.key.repeat)
		{
			switch(e.key.keysym.sym)
			{
			case SDLK_F1:
				ToggleFullScreen();
				break;
			case SDLK_ESCAPE:
				quit_ = true;
				break;
			/* Player Keys */
			case SDLK_LEFT:
				leftDown = true;
				bg->SetScroll(Right);
				break;
			case SDLK_RIGHT:
				rightDown = true;
				bg->SetScroll(Left);
				break;
			case SDLK_UP:
				upDown = true;
				bg->SetScroll(player->GetDirection() == Right? 
					Left : Right);
				break;
			case SDLK_DOWN:
				downDown = true;
				bg->SetScroll(player->GetDirection() == Right?
					Left : Right);
				break;
			case SDLK_SPACE:
				player->Jump(1, 20);
				break;
			}
		}
		else if(e.key.state == SDL_RELEASED && !e.key.repeat)
		{
			switch(e.key.keysym.sym)
			{
			case SDLK_LEFT:
				leftDown = false;
				Stop();
				break;
			case SDLK_RIGHT:
				rightDown = false;
				Stop();
				break;
			case SDLK_UP:
				upDown = false;
				Stop();
				break;
			case SDLK_DOWN:
				downDown = false;
				Stop();
				break;
			}
		}
	}

	virtual void Update() override
	{
		//Update movement vectors
		if (upDown) player->GoUp(*this);
		if (downDown) player->GoDown(*this);
		if (rightDown) player->GoRight(*this);
		if (leftDown) player->GoLeft(*this);
		bg->SetScroll(upDown || downDown || rightDown || leftDown);

		//Other game logic
		gameObjects.Update(*this);
	}

	virtual void Render() override
	{
		SDL_RenderClear( renderer_ );
	
		gameObjects.Draw( renderer_ );
	
		SDL_RenderPresent( renderer_ );
	}


	const RectF& MoveBounds() const
	{
		return moveBounds;
	}


private:
	void Stop()
	{
		player->Stop();
		bg->Stop();
	}

};
