#include "Game.h"


	Game::Game() 
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

	Game::~Game()
	{
		logPrintf("Game object released");
	}

	bool Game::Init()
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

	void Game::ProcessEvent(const SDL_Event& e)
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

	
	
	void Game::Update()
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



	void Game::Render()
	{
		SDL_RenderClear( renderer_ );
	
		gameObjects.Draw( renderer_ );
	
		SDL_RenderPresent( renderer_ );
	}



	void Game::Stop()
	{
		player->Stop();
		bg->Stop();
	}

