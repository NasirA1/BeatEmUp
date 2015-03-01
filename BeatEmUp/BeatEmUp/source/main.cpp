#include <stdio.h>
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
	Game() 
		: SDLApp(SCREEN_WIDTH
		, SCREEN_HEIGHT
		, "Nasir's Beat 'em Up Game")
		, bg(NULL) 
		, leftDown(false)
		, rightDown(false)
		, upDown(false)
		, downDown(false)
	{
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
		gameObjects.push_back(bg);
		gameObjects.push_back(player);

		gameObjects.push_back(new Rock("resources/rock.png", renderer_));

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
				bg->SetScroll(player->GetDirection() == Right ? Left : Right);
				break;
			case SDLK_DOWN:
				downDown = true;
				bg->SetScroll(player->GetDirection() == Right ? Left : Right);
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
		if (upDown) player->GoUp();
		if (downDown) player->GoDown();
		if (rightDown) player->GoRight();
		if (leftDown) player->GoLeft();
		bg->SetScroll(upDown || downDown || rightDown || leftDown);

		//Other game logic
		gameObjects.Update();
	}

	virtual void Render() override
	{
		SDL_RenderClear( renderer_ );
	
		gameObjects.Draw( renderer_ );
	
		SDL_RenderPresent( renderer_ );
	}

	void Stop()
	{
		player->Stop();
		bg->Stop();
	}

private:
	GameObjectList gameObjects;
	Background* bg;
	Player* player;

	bool leftDown;
	bool rightDown;
	bool upDown;
	bool downDown;
};



int main( int argc, char* args[] )
{

	{
		Game game;

		if(game.Init())
		{
			game.Run();
		}
	}
	
#ifdef _DEBUG
	//getchar();
#endif
	return 0;
}


#pragma region Balls Demo
#if 0
class Game : public SDLApp
{
public:
	Game();
	virtual ~Game();
	virtual bool Init() override;
	virtual void ProcessEvent(const SDL_Event& e) override;
	virtual void Update() override;
	virtual void Render() override;

private:
	GameObjectList gameObjects;
};


Game::Game() 
	: SDLApp(SCREEN_WIDTH
	, SCREEN_HEIGHT
	, "Balls Demo")
{
}


Game::~Game()
{
	logPrintf("Game object destroyed");
}


bool Game::Init()
{
	if(!SDLApp::Init())
		return false;

	Ball::AddRandomBalls(gameObjects, 20);

	return true;
}


void Game::ProcessEvent(const SDL_Event& e)
{
	switch(e.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		quit_ = true;
		break;
	}
}


void Game::Update()
{
	gameObjects.Update();
}


void Game::Render()
{
	//Clear screen
	SDL_RenderClear( renderer_ );

	//Render texture to screen
	//SDL_RenderCopy( renderer_, gTexture, NULL, NULL );
	gameObjects.Draw( renderer_ );

	//Update screen
	SDL_RenderPresent( renderer_ );
}

#endif
#pragma endregion
