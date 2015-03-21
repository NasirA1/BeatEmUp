#include "Game.h"
#include <sstream>
#include <algorithm>
#include "Mixer.h"


Game::Game() 
	: SDLApp(SCREEN_WIDTH
	, SCREEN_HEIGHT
	, "Nasir's Beat 'em Up Game")
	, MoveBounds(0.0f, 370.0f, (float)SCREEN_WIDTH, 120.0f)
	, bg(NULL)
	, tbFps(NULL), tbPlayerPos(NULL), tbEnemyPos(NULL)
	, player(NULL)
	, leftDown(false)
	, rightDown(false)
	, upDown(false)
	, downDown(false)
{
}



Game::~Game()
{
	logPrintf("Game object released");
}



bool Game::Init()
{
	if(!SDLApp::Init())
		return false;

	//optimisation - set desired size
	gameObjects.reserve(20);
	enemies.reserve(20);

	bg = new Background(clientWidth_, clientHeight_, renderer_);
	
	Roamer* skaterboy = new Roamer(renderer_, 
		Sprite::FromFile("resources/skater_left.png", renderer_, 71, 90, 11, 0),
		Sprite::FromFile("resources/skater_right.png", renderer_, 71, 90, 11, 0), 
		-200, 390, -200, 1000, true);

	Roamer* knight1 = new Roamer(renderer_, 
		Sprite::FromFile("resources/knightwalk_left.png", renderer_, 128, 128, 4, 15),
		Sprite::FromFile("resources/knightwalk_right.png", renderer_, 128, 128, 4, 3), 
		5000, 480, -5000, 5000, false);

	//enemies.push_back(new Andore(renderer_, 1200, 450));
	//enemies.push_back(new Andore(renderer_, 2400, 450));
	//enemies.push_back(new Joker(renderer_, 300, 400));
	enemies.push_back(new Axl(renderer_, 300, 400));

	player = new Player(renderer_);

	tbFps = new TextBlock("FPS: 00.000000", 16, 0.0f, 0.0f, renderer_);	
	tbPlayerPos = new TextBlock("Pos {}", 16, 0.0f, tbFps->Position().bottom() + 1, renderer_);
	tbEnemyPos = new TextBlock("Enemy Pos {}", 16, 0.0f, tbPlayerPos->Position().bottom() + 1, renderer_);

	gameObjects.push_back(bg);
	gameObjects.push_back(tbFps);
	gameObjects.push_back(tbPlayerPos);
	gameObjects.push_back(tbEnemyPos);
	gameObjects.push_back(player);
	gameObjects.push_back(skaterboy);
	gameObjects.push_back(knight1);
	for(unsigned int i = 0; i < enemies.size(); ++i)
		gameObjects.push_back(enemies[i]);
	gameObjects.push_back(new Rock("resources/rock.png", renderer_));

	MIXER.Instance();
	//MIXER.Play(Mixer::ST_Track1);
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
			bg->SetScroll(player->GetDirection() == Right? Left : Right);
			break;
		case SDLK_DOWN:
			downDown = true;
			bg->SetScroll(player->GetDirection() == Right? Left : Right);
			break;
		case SDLK_SPACE:
			player->Jump();
			break;
		case SDLK_a:
			player->Punch();
			break;
		case SDLK_s:
			player->Kick();
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
	if (upDown) player->GoUp();
	if (downDown) player->GoDown();
	if (rightDown) player->GoRight();
	if (leftDown) player->GoLeft();
	bg->SetScroll(player->GetState() == Player::PS_Walking
		|| player->GetState() == Player::PS_Jumping);

	//text
	std::stringstream ss;
	ss << "FPS: " << Fps();
	tbFps->SetText(ss.str());
	ss.str("");
	ss.clear();
	ss << "Pos: {" << (int)player->Position().x 
		<< "," << (int)player->Position().y 
		<< "," << (int)player->Position().z 
		<< "}  Health: " << player->GetHealth();
	tbPlayerPos->SetText(ss.str());
	//ss.str("");
	//ss.clear();
	//ss << "Enemy Pos: {" << (int)andore->Position().x 
	//	<< "," << (int)andore->Position().y 
	//	<< "," << (int)andore->Position().z 
	//	<< "}  Health: " << andore->GetHealth();
	//tbEnemyPos->SetText(ss.str());


	//Other game logic
	gameObjects.Update();
}



void Game::Render()
{
	//SDL_RenderClear( renderer_ );
	//Sort by depth, then draw
	std::sort(gameObjects.begin(), gameObjects.end(), GameObjectSortByDepth());
	gameObjects.Draw( renderer_ );
	SDL_RenderPresent( renderer_ );
}



void Game::Stop()
{
	player->Stop();
	bg->Stop();
}

